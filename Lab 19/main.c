#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#ifdef _OPENMP
# include <omp.h>
#endif

// Lab 19 - Normalize a signal of length N in the 2-norm and the max norm
// using fine grain and coarse grain parallelism with OpenMP

int main(int argc, char* argv[])
{
    void usage(const char* prog_name);
    void make_signal(vector* x, const int thread_count);

    if (argc != 3) { usage(argv[0]); }
    const int thread_count = strtol(argv[1], NULL, 10);
    const int N = strtol(argv[2], NULL, 10);
    if (thread_count < 1 || N < 1 || N % thread_count != 0)
    { usage(argv[0]); }

    vector x = new_vector(N);
    double time1, time2;

    printf("\n N = %i, threads = %i\n\n", N, thread_count);

    // ---------- Fine grain: 2-norm ----------
    make_signal(&x, thread_count);
    double sum = 0.0;
    time1 = omp_get_wtime();

# pragma omp parallel for num_threads(thread_count) reduction(+:sum)
    for (int i=1; i<=N; i++)
    { sum += vget(x,i)*vget(x,i); }

    double norm2 = sqrt(sum);

# pragma omp parallel for num_threads(thread_count)
    for (int i=1; i<=N; i++)
    { vget(x,i) = vget(x,i)/norm2; }

    time2 = omp_get_wtime();
    printf(" Fine grain,   2-norm   = %12.5e, time = %12.5e\n", norm2, time2-time1);

    // ---------- Fine grain: max norm ----------
    make_signal(&x, thread_count);
    double normmax = 0.0;
    time1 = omp_get_wtime();

# pragma omp parallel for num_threads(thread_count) reduction(max:normmax)
    for (int i=1; i<=N; i++)
    {
        if (fabs(vget(x,i)) > normmax)
        { normmax = fabs(vget(x,i)); }
    }

# pragma omp parallel for num_threads(thread_count)
    for (int i=1; i<=N; i++)
    { vget(x,i) = vget(x,i)/normmax; }

    time2 = omp_get_wtime();
    printf(" Fine grain,   max norm = %12.5e, time = %12.5e\n", normmax, time2-time1);

    // ---------- Coarse grain: 2-norm ----------
    make_signal(&x, thread_count);
    sum = 0.0;
    time1 = omp_get_wtime();

# pragma omp parallel num_threads(thread_count)
    {
        const int my_rank = omp_get_thread_num();
        const int N_per_thread = N/thread_count;
        const int istart = my_rank*N_per_thread + 1;
        const int iend = (my_rank+1)*N_per_thread;

        double sum_thread = 0.0;
        for (int i=istart; i<=iend; i++)
        { sum_thread += vget(x,i)*vget(x,i); }

#     pragma omp critical
        sum += sum_thread;

#     pragma omp barrier // needed here

        const double norm_all = sqrt(sum);
        for (int i=istart; i<=iend; i++)
        { vget(x,i) = vget(x,i)/norm_all; }
    }

    norm2 = sqrt(sum);
    time2 = omp_get_wtime();
    printf(" Coarse grain, 2-norm   = %12.5e, time = %12.5e\n", norm2, time2-time1);

    // ---------- Coarse grain: max norm ----------
    make_signal(&x, thread_count);
    normmax = 0.0;
    time1 = omp_get_wtime();

# pragma omp parallel num_threads(thread_count)
    {
        const int my_rank = omp_get_thread_num();
        const int N_per_thread = N/thread_count;
        const int istart = my_rank*N_per_thread + 1;
        const int iend = (my_rank+1)*N_per_thread;

        double max_thread = 0.0;
        for (int i=istart; i<=iend; i++)
        {
            if (fabs(vget(x,i)) > max_thread)
            { max_thread = fabs(vget(x,i)); }
        }

#     pragma omp critical
        if (max_thread > normmax)
        { normmax = max_thread; }

#     pragma omp barrier // needed here

        for (int i=istart; i<=iend; i++)
        { vget(x,i) = vget(x,i)/normmax; }
    }

    time2 = omp_get_wtime();
    printf(" Coarse grain, max norm = %12.5e, time = %12.5e\n\n", normmax, time2-time1);

    delete_vector(&x);
    return 0;
}

void usage(const char *prog_name)
{
    fprintf(stderr, "usage: %s <num_threads> <N>\n", prog_name);
    fprintf(stderr, " num_threads should be positive\n");
    fprintf(stderr, " N should be positive\n");
    fprintf(stderr, " mod(N,num_threads) should be 0\n");
    exit(1);
}

// the given signal: x_i = sin(i/1000) + 0.5*cos(i/300)
void make_signal(vector* x, const int thread_count)
{
    const int N = x->size;
# pragma omp parallel for num_threads(thread_count)
    for (int i=1; i<=N; i++)
    { vgetp(x,i) = sin(i/1000.0) + 0.5*cos(i/300.0); }
}
