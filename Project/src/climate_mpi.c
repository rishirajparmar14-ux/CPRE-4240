/*
 * climate_mpi.c - the MPI (parallel) version of the project.
 *
 * Same analysis as climate_serial.c, but the stations are split between
 * processes so they are analyzed at the same time:
 *
 *   1. Rank 0 reads the station list.
 *   2. MPI_Bcast   - everyone learns how many stations and regions there are.
 *   3. MPI_Scatterv - each rank gets its own block of stations.
 *   4. Each rank analyzes its stations and adds up its own region totals.
 *   5. MPI_Gatherv - all station results go back to rank 0 (in order).
 *   6. MPI_Reduce  - the region totals from all ranks are added together.
 *   7. Rank 0 writes the same two CSV files as the serial version.
 *
 * Usage:
 *   mpirun -np 4 ./climate_mpi <station_list> <data_dir> <output_prefix>
 * Example:
 *   mpirun -np 4 ./climate_mpi stations_test.txt data results/mpi4
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "ghcn.h"

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 4) {
        if (rank == 0)
            printf("Usage: mpirun -np N %s <station_list> <data_dir> <output_prefix>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    /* ---- 1. Rank 0 reads the station list ---- */
    StationJob *all_jobs = NULL;
    char (*regions)[REGION_LEN] = NULL;
    int n = 0, num_regions = 0;
    if (rank == 0) {
        n = read_station_list(argv[1], &all_jobs, &regions, &num_regions);
        if (n <= 0) n = 0;
        printf("%d stations, %d regions, %d processes\n", n, num_regions, size);
    }

    /* ---- 2. Tell everyone the number of stations and regions ---- */
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_regions, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (n == 0) { MPI_Finalize(); return 1; }

    /* How many stations each rank gets. If n doesn't divide evenly,
     * the first (n % size) ranks get one extra.
     * Counts are in BYTES because we send the structs as MPI_BYTE. */
    int *counts = malloc(size * sizeof(int));   /* stations per rank     */
    int *job_bytes = malloc(size * sizeof(int));
    int *job_displs = malloc(size * sizeof(int));
    int *res_bytes = malloc(size * sizeof(int));
    int *res_displs = malloc(size * sizeof(int));
    int offset = 0;
    for (int r = 0; r < size; r++) {
        counts[r] = n / size + (r < n % size ? 1 : 0);
        job_bytes[r]  = counts[r] * sizeof(StationJob);
        job_displs[r] = offset * sizeof(StationJob);
        res_bytes[r]  = counts[r] * sizeof(StationResult);
        res_displs[r] = offset * sizeof(StationResult);
        offset += counts[r];
    }
    int my_count = counts[rank];

    StationJob    *my_jobs    = malloc((my_count > 0 ? my_count : 1) * sizeof(StationJob));
    StationResult *my_results = malloc((my_count > 0 ? my_count : 1) * sizeof(StationResult));
    double *my_totals  = calloc(num_regions * R_FIELDS, sizeof(double));
    double *all_totals = calloc(num_regions * R_FIELDS, sizeof(double));
    StationResult *all_results = (rank == 0) ? malloc(n * sizeof(StationResult)) : NULL;

    /* Start the clock when every process is ready */
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    /* ---- 3. Send each rank its block of stations ---- */
    MPI_Scatterv(all_jobs, job_bytes, job_displs, MPI_BYTE,
                 my_jobs, job_bytes[rank], MPI_BYTE, 0, MPI_COMM_WORLD);

    /* ---- 4. Each rank analyzes its own stations ---- */
    double work_start = MPI_Wtime();
    for (int i = 0; i < my_count; i++) {
        analyze_station(argv[2], &my_jobs[i], &my_results[i]);
        add_to_regions(&my_results[i], my_totals);
    }
    double my_time = MPI_Wtime() - work_start;

    /* ---- 5. Collect all station results on rank 0 ---- */
    MPI_Gatherv(my_results, res_bytes[rank], MPI_BYTE,
                all_results, res_bytes, res_displs, MPI_BYTE, 0, MPI_COMM_WORLD);

    /* ---- 6. Add up the region totals from every rank ---- */
    MPI_Reduce(my_totals, all_totals, num_regions * R_FIELDS, MPI_DOUBLE,
               MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double elapsed = MPI_Wtime() - start;

    /* How long did each rank work? (shows if the work was balanced) */
    double *rank_times = (rank == 0) ? malloc(size * sizeof(double)) : NULL;
    MPI_Gather(&my_time, 1, MPI_DOUBLE, rank_times, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* ---- 7. Rank 0 writes the results ---- */
    if (rank == 0) {
        write_results(argv[3], all_results, n, regions, num_regions, all_totals);

        int used = 0;
        for (int i = 0; i < n; i++) used += all_results[i].ok;

        double tmin = rank_times[0], tmax = rank_times[0], tsum = 0;
        for (int r = 0; r < size; r++) {
            if (rank_times[r] < tmin) tmin = rank_times[r];
            if (rank_times[r] > tmax) tmax = rank_times[r];
            tsum += rank_times[r];
        }
        printf("Stations used: %d of %d\n", used, n);
        printf("Work per rank: min %.3f s, avg %.3f s, max %.3f s\n",
               tmin, tsum / size, tmax);
        printf("Time: %.3f seconds\n", elapsed);

        free(all_jobs); free(regions); free(all_results); free(rank_times);
    }

    free(counts); free(job_bytes); free(job_displs); free(res_bytes); free(res_displs);
    free(my_jobs); free(my_results); free(my_totals); free(all_totals);
    MPI_Finalize();
    return 0;
}
