/*
 * ghcn.c - reading GHCN-Daily .dly files and doing the per-station math.
 *
 * .dly file format (one line = one station, one month, one measurement):
 *   columns  1-11  station ID
 *   columns 12-15  year
 *   columns 16-17  month
 *   columns 18-21  element (TMAX, TMIN, PRCP, ...)
 *   then 31 days, each 8 characters:
 *       5 chars value (tenths of degrees C for temperature, -9999 = missing)
 *       1 char  measurement flag
 *       1 char  quality flag (blank = passed NOAA's quality checks)
 *       1 char  source flag
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ghcn.h"

/* Turn columns [start, start+len) of a line into an int (columns from 0). */
static int field_int(const char *line, int start, int len)
{
    char buf[16];
    memcpy(buf, line + start, len);
    buf[len] = '\0';
    return atoi(buf);
}

int read_station_list(const char *path, StationJob **jobs,
                      char (**regions)[REGION_LEN], int *num_regions)
{
    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return -1; }

    int cap = 1024, n = 0;
    int rcap = 64, nr = 0;
    StationJob *list = malloc(cap * sizeof *list);
    char (*names)[REGION_LEN] = malloc(rcap * sizeof *names);

    char id[ID_LEN], region[REGION_LEN];
    /* %11s and %7s stop at the size of each field, so nothing overflows */
    while (fscanf(f, "%11s %7s", id, region) == 2) {
        if (n == cap) { cap *= 2; list = realloc(list, cap * sizeof *list); }

        /* find this region in the list, or add it */
        int r;
        for (r = 0; r < nr; r++)
            if (strcmp(names[r], region) == 0) break;
        if (r == nr) {
            if (nr == rcap) { rcap *= 2; names = realloc(names, rcap * sizeof *names); }
            strcpy(names[nr], region);
            nr++;
        }

        strcpy(list[n].id, id);
        list[n].region = r;
        n++;
    }
    fclose(f);

    *jobs = list;
    *regions = names;
    *num_regions = nr;
    return n;
}

void analyze_station(const char *data_dir, const StationJob *job,
                     StationResult *res)
{
    /* Running sums for every year and month in our range. */
    static double sum_max[NUM_YEARS][12], sum_min[NUM_YEARS][12];
    static int    n_max[NUM_YEARS][12],   n_min[NUM_YEARS][12];
    static int    hot[NUM_YEARS];
    memset(sum_max, 0, sizeof sum_max);  memset(sum_min, 0, sizeof sum_min);
    memset(n_max, 0, sizeof n_max);      memset(n_min, 0, sizeof n_min);
    memset(hot, 0, sizeof hot);

    memset(res, 0, sizeof *res);
    snprintf(res->id, ID_LEN, "%s", job->id);
    res->region = job->region;

    char path[512];
    snprintf(path, sizeof path, "%s/%s.dly", data_dir, job->id);
    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, "warning: cannot open %s\n", path); return; }

    /* ---- Step 1: read the file and add up the daily values ---- */
    char line[512];
    while (fgets(line, sizeof line, f)) {
        if (strlen(line) < 269) continue;           /* short / bad line */

        int is_max = (strncmp(line + 17, "TMAX", 4) == 0);
        int is_min = (strncmp(line + 17, "TMIN", 4) == 0);
        if (!is_max && !is_min) continue;            /* skip rain, snow... */

        int year  = field_int(line, 11, 4);
        int month = field_int(line, 15, 2);
        if (year < FIRST_YEAR || year > LAST_YEAR || month < 1 || month > 12)
            continue;
        int y = year - FIRST_YEAR, m = month - 1;

        for (int day = 0; day < 31; day++) {
            int pos   = 21 + day * 8;
            int value = field_int(line, pos, 5);
            char qflag = line[pos + 6];
            if (value == -9999 || qflag != ' ') continue; /* missing or bad */

            if (is_max) {
                sum_max[y][m] += value / 10.0;
                n_max[y][m]++;
                if (value > HOT_DAY_TENTHS) hot[y]++;
            } else {
                sum_min[y][m] += value / 10.0;
                n_min[y][m]++;
            }
        }
    }
    fclose(f);

    /* ---- Step 2: yearly averages for years with all 12 months ---- */
    double year_mean[NUM_YEARS];
    int    year_ok[NUM_YEARS];
    for (int y = 0; y < NUM_YEARS; y++) {
        year_ok[y] = 1;
        double total = 0.0;
        for (int m = 0; m < 12; m++) {
            if (n_max[y][m] < MIN_DAYS_MONTH || n_min[y][m] < MIN_DAYS_MONTH) {
                year_ok[y] = 0;
                break;
            }
            /* monthly mean = average of (mean high + mean low) / 2 */
            total += (sum_max[y][m] / n_max[y][m] +
                      sum_min[y][m] / n_min[y][m]) / 2.0;
        }
        year_mean[y] = year_ok[y] ? total / 12.0 : 0.0;
    }

    /* ---- Step 3: warming trend = slope of a straight-line fit ---- */
    double sx = 0, sy = 0;
    int count = 0;
    for (int y = 0; y < NUM_YEARS; y++)
        if (year_ok[y]) { sx += y; sy += year_mean[y]; count++; }
    res->good_years = count;
    if (count < MIN_YEARS_TREND) return;            /* not enough data */

    double xbar = sx / count, ybar = sy / count, sxy = 0, sxx = 0;
    for (int y = 0; y < NUM_YEARS; y++)
        if (year_ok[y]) {
            sxy += (y - xbar) * (year_mean[y] - ybar);
            sxx += (y - xbar) * (y - xbar);
        }
    res->trend = (sxy / sxx) * 10.0;                /* per year -> per decade */

    /* ---- Step 4: compare the early and late 30-year periods ---- */
    double mean_e = 0, mean_l = 0, hot_e = 0, hot_l = 0;
    int n_e = 0, n_l = 0;
    for (int y = 0; y < NUM_YEARS; y++) {
        if (!year_ok[y]) continue;
        int year = FIRST_YEAR + y;
        if (year >= EARLY_START && year <= EARLY_END) {
            mean_e += year_mean[y]; hot_e += hot[y]; n_e++;
        }
        if (year >= LATE_START && year <= LATE_END) {
            mean_l += year_mean[y]; hot_l += hot[y]; n_l++;
        }
    }
    if (n_e < MIN_YEARS_PERIOD || n_l < MIN_YEARS_PERIOD) return;

    res->mean_early = mean_e / n_e;
    res->mean_late  = mean_l / n_l;
    res->hot_early  = hot_e / n_e;
    res->hot_late   = hot_l / n_l;
    res->ok = 1;
}

void add_to_regions(const StationResult *res, double *totals)
{
    if (!res->ok) return;
    double *t = totals + res->region * R_FIELDS;
    t[R_COUNT]       += 1.0;
    t[R_TREND]       += res->trend;
    t[R_MEAN_CHANGE] += res->mean_late - res->mean_early;
    t[R_HOT_EARLY]   += res->hot_early;
    t[R_HOT_LATE]    += res->hot_late;
}

int write_results(const char *prefix, const StationResult *results, int n,
                  char (*regions)[REGION_LEN], int num_regions,
                  const double *totals)
{
    char path[512];

    /* one row per station */
    snprintf(path, sizeof path, "%s_stations.csv", prefix);
    FILE *f = fopen(path, "w");
    if (!f) { perror(path); return -1; }
    /* Column names:
     *   years       = number of complete years
     *   trend       = warming in degrees C per decade
     *   temp_before = average temperature 1951-1980 (C)
     *   temp_now    = average temperature 1996-2025 (C)
     *   hot_before  = days above 35 C per year, 1951-1980
     *   hot_now     = days above 35 C per year, 1996-2025 */
    fprintf(f, "station,region,used,years,trend,temp_before,temp_now,"
               "hot_before,hot_now\n");
    for (int i = 0; i < n; i++) {
        const StationResult *r = &results[i];
        fprintf(f, "%s,%s,%s,%d,%.4f,%.3f,%.3f,%.2f,%.2f\n",
                r->id, regions[r->region], r->ok ? "yes" : "no",
                r->good_years, r->trend, r->mean_early, r->mean_late,
                r->hot_early, r->hot_late);
    }
    fclose(f);

    /* one row per region (averages of the stations in it) */
    snprintf(path, sizeof path, "%s_regions.csv", prefix);
    f = fopen(path, "w");
    if (!f) { perror(path); return -1; }
    /* Column names (averages over the stations in each region):
     *   trend      = warming in degrees C per decade
     *   warming    = temperature change from 1951-1980 to 1996-2025 (C)
     *   hot_before = days above 35 C per year, 1951-1980
     *   hot_now    = days above 35 C per year, 1996-2025
     *   hot_change = hot_now - hot_before */
    fprintf(f, "region,stations,trend,warming,hot_before,hot_now,hot_change\n");
    for (int r = 0; r < num_regions; r++) {
        const double *t = totals + r * R_FIELDS;
        if (t[R_COUNT] == 0) continue;
        double c = t[R_COUNT];
        fprintf(f, "%s,%d,%.3f,%.2f,%.2f,%.2f,%+.2f\n",
                regions[r], (int)c, t[R_TREND] / c, t[R_MEAN_CHANGE] / c,
                t[R_HOT_EARLY] / c, t[R_HOT_LATE] / c,
                (t[R_HOT_LATE] - t[R_HOT_EARLY]) / c);
    }
    fclose(f);
    return 0;
}
