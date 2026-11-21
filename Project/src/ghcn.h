/*
 * ghcn.h - shared code for reading NOAA GHCN-Daily station files
 *          and calculating warming trends and hot days.
 *
 * Used by both climate_serial.c and climate_mpi.c so the two versions
 * do exactly the same math for each station.
 */
#ifndef GHCN_H
#define GHCN_H

/* ----- Settings for the analysis (change these if you want) ----- */
#define FIRST_YEAR      1950    /* first year we look at                    */
#define LAST_YEAR       2025    /* last year we look at                     */
#define NUM_YEARS       (LAST_YEAR - FIRST_YEAR + 1)

#define EARLY_START     1951    /* "before" period for hot days             */
#define EARLY_END       1980
#define LATE_START      1996    /* "now" period for hot days                */
#define LATE_END        2025

#define HOT_DAY_TENTHS  350     /* 35.0 C, stored in tenths of a degree     */
#define MIN_DAYS_MONTH  20      /* a month needs this many readings         */
#define MIN_YEARS_TREND 50      /* a station needs this many good years     */
#define MIN_YEARS_PERIOD 20     /* each 30-year period needs this many      */

#define ID_LEN          12      /* 11-character station ID + '\0'           */
#define REGION_LEN      8       /* e.g. "US-IA" or "CA" + '\0'              */

/* One station to analyze: its ID and which region it belongs to. */
typedef struct {
    char id[ID_LEN];
    int  region;                /* index into the region name list          */
} StationJob;

/* The results for one station. */
typedef struct {
    char   id[ID_LEN];
    int    region;
    int    ok;                  /* 1 if the station had enough good data    */
    int    good_years;          /* years with all 12 months complete        */
    double trend;               /* warming rate in degrees C per decade     */
    double mean_early;          /* average temperature, EARLY period (C)    */
    double mean_late;           /* average temperature, LATE period (C)     */
    double hot_early;           /* hot days per year, EARLY period          */
    double hot_late;            /* hot days per year, LATE period           */
} StationResult;

/* Per-region totals. Stored as plain doubles so MPI_Reduce can add them. */
enum { R_COUNT, R_TREND, R_MEAN_CHANGE, R_HOT_EARLY, R_HOT_LATE, R_FIELDS };

/* Read the station list made by select_stations.
 * Fills *jobs and *regions (both malloc'd). Returns the number of stations,
 * or -1 on error. */
int read_station_list(const char *path, StationJob **jobs,
                      char (**regions)[REGION_LEN], int *num_regions);

/* Read data_dir/<id>.dly and fill in *res. */
void analyze_station(const char *data_dir, const StationJob *job,
                     StationResult *res);

/* Add one station's result into the region totals array
 * (num_regions * R_FIELDS doubles). */
void add_to_regions(const StationResult *res, double *totals);

/* Write the two output CSV files: <prefix>_stations.csv and
 * <prefix>_regions.csv */
int write_results(const char *prefix, const StationResult *results, int n,
                  char (*regions)[REGION_LEN], int num_regions,
                  const double *totals);

#endif
