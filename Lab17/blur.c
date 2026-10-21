#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"
#ifdef _OPENMP
# include <omp.h>
#endif

// Lab 17 - Image blurring with a 3x3 filter using OpenMP
// Random N x N image, img(i,j) = rand() % 256
// Blurred image is (N-2) x (N-2), each pixel is the average of itself and its 8 neighbors

int main(int argc, char* argv[])
{
    void usage(const char* prog_name);

    if (argc != 3) { usage(argv[0]); }
    const int thread_count = strtol(argv[1], NULL, 10);
    const int N = strtol(argv[2], NULL, 10);
    if (thread_count <= 0 || N < 3)
    { usage(argv[0]); }

    // random image
    srand( time(NULL) );
    matrix img = new_matrix(N,N);
    for (int i=1; i<=N; i++)
    for (int j=1; j<=N; j++)
    { mget(img,i,j) = (double)(rand() % 256); }

    // blurred image
    matrix blur = new_matrix(N-2,N-2);

    const double time1 = omp_get_wtime();

# pragma omp parallel for num_threads(thread_count)
    for (int i=2; i<=N-1; i++)
    {
        for (int j=2; j<=N-1; j++)
        {
            double sum = 0.0;
            for (int a=i-1; a<=i+1; a++)
            for (int b=j-1; b<=j+1; b++)
            { sum += mget(img,a,b); }
            mget(blur,i-1,j-1) = sum/9.0;
        }
    }

    const double time2 = omp_get_wtime();
    const double clock_time = time2-time1;

    // print the images only if they are small
    if (N <= 8)
    {
        print_matrix(&img);
        print_matrix(&blur);
    }

    printf(" N = %i, With %i threads, clock_time = %11.5e (sec)\n",
           N, thread_count, clock_time);

    delete_matrix(&img);
    delete_matrix(&blur);
    return 0;
}

void usage(const char *prog_name)
{
    fprintf(stderr, "usage: %s <thread_count> <N>\n", prog_name);
    fprintf(stderr, " thread_count should be positive\n");
    fprintf(stderr, " N should be at least 3\n");
    exit(1);
}
