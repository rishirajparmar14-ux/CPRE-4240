/*
 * climate_mpi_dynamic.c - MPI version with DYNAMIC load balancing.
 *
 * Problem with climate_mpi.c (static): each rank gets one fixed block of
 * stations. Some stations have much bigger files than others, so some ranks
 * finish early and wait for the slowest one.
 *
 * Fix (manager / worker):
 *   - Rank 0 is the MANAGER. It does not analyze stations; it hands out
 *     small chunks of stations to whichever worker asks for more work.
 *   - Ranks 1..N-1 are WORKERS. Each one asks for a chunk, analyzes it,
 *     sends the results back, and asks again, until the manager says STOP.
 *   - Fast workers simply get more chunks, so everyone finishes at about
 *     the same time.
 *
 * Messages:
 *   worker -> manager  TAG_DONE    int: start index of the chunk it finished
 *                                  (-1 the first time, when it has none yet)
 *   worker -> manager  TAG_RESULT  the StationResults of that chunk
 *   manager -> worker  TAG_WORK    int: start index of the next chunk
 *   manager -> worker  TAG_STOP    no more work
 *
 * Usage:
 *   mpirun -np 4 ./climate_mpi_dynamic <station_list> <data_dir> <output_prefix> [chunk]
 *   chunk = stations per message (default 4)
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "ghcn.h"

#define TAG_DONE   1
#define TAG_RESULT 2
#define TAG_WORK   3
#define TAG_STOP   4

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 4) {
        if (rank == 0)
            printf("Usage: mpirun -np N %s <station_list> <data_dir> <output_prefix> [chunk]\n", argv[0]);
        MPI_Finalize();
        return 1;
    }
    int chunk = (argc > 4) ? atoi(argv[4]) : 4;
    if (chunk < 1) chunk = 1;

    /* ---- Rank 0 reads the station list, everyone gets a copy ---- */
    StationJob *jobs = NULL;
    char (*regions)[REGION_LEN] = NULL;
    int n = 0, num_regions = 0;
    if (rank == 0) {
        n = read_station_list(argv[1], &jobs, &regions, &num_regions);
        if (n < 0) n = 0;
        printf("%d stations, %d regions, %d processes (%d workers), chunk %d\n",
               n, num_regions, size, size > 1 ? size - 1 : 1, chunk);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_regions, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (n == 0) { MPI_Finalize(); return 1; }

    /* The job list is small (16 bytes per station), so just send it all.
     * Then the manager only needs to send a start index, not the stations. */
    if (rank != 0) jobs = malloc(n * sizeof(StationJob));
    MPI_Bcast(jobs, n * sizeof(StationJob), MPI_BYTE, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();
    double my_work = 0.0;                   /* time this rank spent analyzing */
    StationResult *all_results = NULL;

    if (size == 1) {
        /* Only one process: no workers, so just do everything here. */
        all_results = malloc(n * sizeof(StationResult));
        double t = MPI_Wtime();
        for (int i = 0; i < n; i++)
            analyze_station(argv[2], &jobs[i], &all_results[i]);
        my_work = MPI_Wtime() - t;
    }
    else if (rank == 0) {
        /* ================= MANAGER ================= */
        all_results = malloc(n * sizeof(StationResult));
        int next = 0;                       /* next station to hand out */
        int stopped = 0;                    /* workers told to stop     */
        while (stopped < size - 1) {
            int done_start;
            MPI_Status st;
            MPI_Recv(&done_start, 1, MPI_INT, MPI_ANY_SOURCE, TAG_DONE,
                     MPI_COMM_WORLD, &st);
            int worker = st.MPI_SOURCE;

            /* if the worker finished a chunk, receive its results in place */
            if (done_start >= 0) {
                int count = (done_start + chunk <= n) ? chunk : n - done_start;
                MPI_Recv(&all_results[done_start], count * sizeof(StationResult),
                         MPI_BYTE, worker, TAG_RESULT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            /* give it more work, or tell it to stop */
            if (next < n) {
                MPI_Send(&next, 1, MPI_INT, worker, TAG_WORK, MPI_COMM_WORLD);
                next += chunk;
            } else {
                MPI_Send(&next, 1, MPI_INT, worker, TAG_STOP, MPI_COMM_WORLD);
                stopped++;
            }
        }
    }
    else {
        /* ================= WORKER ================= */
        StationResult *buf = malloc(chunk * sizeof(StationResult));
        int done_start = -1;                /* nothing finished yet */
        while (1) {
            /* report the last chunk (if any) and ask for more */
            MPI_Send(&done_start, 1, MPI_INT, 0, TAG_DONE, MPI_COMM_WORLD);
            if (done_start >= 0) {
                int count = (done_start + chunk <= n) ? chunk : n - done_start;
                MPI_Send(buf, count * sizeof(StationResult), MPI_BYTE,
                         0, TAG_RESULT, MPI_COMM_WORLD);
            }

            int my_start;
            MPI_Status st;
            MPI_Recv(&my_start, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
            if (st.MPI_TAG == TAG_STOP) break;

            int count = (my_start + chunk <= n) ? chunk : n - my_start;
            double t = MPI_Wtime();
            for (int i = 0; i < count; i++)
                analyze_station(argv[2], &jobs[my_start + i], &buf[i]);
            my_work += MPI_Wtime() - t;
            done_start = my_start;
        }
        free(buf);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double elapsed = MPI_Wtime() - start;

    /* Work time of every rank (the manager's is 0, so we skip it below) */
    double *times = (rank == 0) ? malloc(size * sizeof(double)) : NULL;
    MPI_Gather(&my_work, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        /* Region totals: added up in station order, same as the serial version */
        double *totals = calloc(num_regions * R_FIELDS, sizeof(double));
        int used = 0;
        for (int i = 0; i < n; i++) {
            add_to_regions(&all_results[i], totals);
            used += all_results[i].ok;
        }
        write_results(argv[3], all_results, n, regions, num_regions, totals);

        int first = (size == 1) ? 0 : 1;    /* skip the manager */
        double tmin = times[first], tmax = times[first], tsum = 0;
        for (int r = first; r < size; r++) {
            if (times[r] < tmin) tmin = times[r];
            if (times[r] > tmax) tmax = times[r];
            tsum += times[r];
        }
        printf("Stations used: %d of %d\n", used, n);
        printf("Work per rank: min %.3f s, avg %.3f s, max %.3f s\n",
               tmin, tsum / (size - first), tmax);
        printf("Time: %.3f seconds\n", elapsed);

        free(totals); free(times); free(regions); free(all_results);
    }
    free(jobs);
    MPI_Finalize();
    return 0;
}
