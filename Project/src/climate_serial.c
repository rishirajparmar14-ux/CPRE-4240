/*
 * climate_serial.c - the SERIAL (one process) version of the project.
 *
 * For every station in the list:
 *   - read its .dly file
 *   - calculate the warming trend and hot days (see ghcn.c)
 * Then average the results for each region and write two CSV files.
 *
 * Usage:
 *   ./climate_serial <station_list> <data_dir> <output_prefix>
 * Example:
 *   ./climate_serial stations_test.txt data results/serial
 *   -> results/serial_stations.csv and results/serial_regions.csv
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ghcn.h"

/* current time in seconds, for timing */
static double now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        printf("Usage: %s <station_list> <data_dir> <output_prefix>\n", argv[0]);
        return 1;
    }

    /* 1. Read the list of stations and their regions */
    StationJob *jobs;
    char (*regions)[REGION_LEN];
    int num_regions;
    int n = read_station_list(argv[1], &jobs, &regions, &num_regions);
    if (n <= 0) { printf("No stations found in %s\n", argv[1]); return 1; }
    printf("%d stations, %d regions\n", n, num_regions);

    StationResult *results = malloc(n * sizeof(StationResult));
    double *totals = calloc(num_regions * R_FIELDS, sizeof(double));

    /* 2. Analyze every station, one after another (this is what MPI will split up) */
    double start = now();
    for (int i = 0; i < n; i++) {
        analyze_station(argv[2], &jobs[i], &results[i]);
        add_to_regions(&results[i], totals);
    }
    double elapsed = now() - start;

    /* 3. Save the results */
    write_results(argv[3], results, n, regions, num_regions, totals);

    int used = 0;
    for (int i = 0; i < n; i++) used += results[i].ok;
    printf("Stations used: %d of %d\n", used, n);
    printf("Time: %.3f seconds\n", elapsed);

    free(jobs); free(regions); free(results); free(totals);
    return 0;
}
