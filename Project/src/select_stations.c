/*
 * select_stations.c - pick the stations to use for the project.
 *
 * Keeps stations that have BOTH daily highs (TMAX) and lows (TMIN)
 * from 1950 (or earlier) to 2025 (or later), and gives each one a region:
 *   US stations  -> "US-" + state   (e.g. US-IA)
 *   other places -> 2-letter country code (first 2 characters of the ID)
 *
 * Usage:
 *   ./select_stations <inventory> <stations> <output> [prefix] [max]
 *   prefix: only keep IDs starting with this (e.g. US), use ALL for every station
 *   max:    keep at most this many stations, spread evenly through the list
 *
 * Example:
 *   ./select_stations data/ghcnd-inventory.txt data/ghcnd-stations.txt stations.txt US 500
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRST_YEAR_NEEDED 1950
#define LAST_YEAR_NEEDED  2025
#define ID_LEN 12                 /* 11 characters + '\0' */

typedef char StationId[ID_LEN];

/* compare two IDs, for qsort and bsearch */
static int compare_ids(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

/* add an ID to a growing list */
static void add_id(StationId **list, int *count, int *capacity, const char *id)
{
    if (*count == *capacity) {
        *capacity *= 2;
        *list = realloc(*list, *capacity * sizeof(StationId));
    }
    strcpy((*list)[*count], id);
    (*count)++;
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: %s <inventory> <stations> <output> [prefix] [max]\n", argv[0]);
        return 1;
    }
    const char *prefix = (argc > 4 && strcmp(argv[4], "ALL") != 0) ? argv[4] : "";
    int max_stations = (argc > 5) ? atoi(argv[5]) : 0;

    /* ---- Step 1: read the inventory, collect good TMAX and TMIN stations ---- */
    FILE *inv = fopen(argv[1], "r");
    if (!inv) { perror(argv[1]); return 1; }

    int cap_max = 1024, cap_min = 1024, n_max = 0, n_min = 0;
    StationId *tmax_ok = malloc(cap_max * sizeof(StationId));
    StationId *tmin_ok = malloc(cap_min * sizeof(StationId));

    char line[256], id[32], element[16];
    double lat, lon;
    int first, last;
    while (fgets(line, sizeof line, inv)) {
        /* inventory line: ID  LATITUDE  LONGITUDE  ELEMENT  FIRSTYEAR  LASTYEAR */
        if (sscanf(line, "%31s %lf %lf %15s %d %d",
                   id, &lat, &lon, element, &first, &last) != 6)
            continue;
        if (strncmp(id, prefix, strlen(prefix)) != 0) continue;
        if (first > FIRST_YEAR_NEEDED || last < LAST_YEAR_NEEDED) continue;

        if (strcmp(element, "TMAX") == 0) add_id(&tmax_ok, &n_max, &cap_max, id);
        if (strcmp(element, "TMIN") == 0) add_id(&tmin_ok, &n_min, &cap_min, id);
    }
    fclose(inv);
    printf("Stations with good TMAX: %d\n", n_max);
    printf("Stations with good TMIN: %d\n", n_min);

    /* ---- Step 2: keep stations that are in BOTH lists ---- */
    qsort(tmax_ok, n_max, sizeof(StationId), compare_ids);
    qsort(tmin_ok, n_min, sizeof(StationId), compare_ids);

    int n_both = 0;
    StationId *both = malloc((n_max > 0 ? n_max : 1) * sizeof(StationId));
    for (int i = 0; i < n_max; i++)
        if (bsearch(tmax_ok[i], tmin_ok, n_min, sizeof(StationId), compare_ids))
            strcpy(both[n_both++], tmax_ok[i]);
    printf("Stations with both:      %d\n", n_both);

    /* ---- Step 3: find the US state for each station ---- */
    char (*state)[3] = calloc(n_both > 0 ? n_both : 1, 3);
    FILE *sta = fopen(argv[2], "r");
    if (!sta) { perror(argv[2]); return 1; }
    while (fgets(line, sizeof line, sta)) {
        /* stations line: ID is columns 1-11, state is columns 39-40 */
        if (strlen(line) < 40) continue;
        StationId sid;
        memcpy(sid, line, 11);
        sid[11] = '\0';
        StationId *found = bsearch(sid, both, n_both, sizeof(StationId), compare_ids);
        if (found) {
            int k = found - both;
            state[k][0] = line[38];
            state[k][1] = line[39];
            state[k][2] = '\0';
        }
    }
    fclose(sta);

    /* ---- Step 4: write the output (spread evenly if there is a max) ---- */
    int n_out = n_both;
    if (max_stations > 0 && max_stations < n_both) n_out = max_stations;

    FILE *out = fopen(argv[3], "w");
    if (!out) { perror(argv[3]); return 1; }
    for (int j = 0; j < n_out; j++) {
        int k = (int)((long)j * n_both / n_out);   /* every (n_both/n_out)-th station */
        char region[8];
        if (strncmp(both[k], "US", 2) == 0 && state[k][0] != ' ' && state[k][0] != '\0')
            snprintf(region, sizeof region, "US-%s", state[k]);
        else
            snprintf(region, sizeof region, "%.2s", both[k]);
        fprintf(out, "%s %s\n", both[k], region);
    }
    fclose(out);
    printf("Wrote %d stations to %s\n", n_out, argv[3]);

    free(tmax_ok); free(tmin_ok); free(both); free(state);
    return 0;
}
