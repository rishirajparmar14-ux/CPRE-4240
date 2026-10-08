#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

#define PI 3.14159265358979323846

// Lab 13 - 1D deblurring using the matrix struct

// random number from a normal distribution with mean 0 and std 1
double randn()
{
    double u1 = (rand() + 1.0)/(RAND_MAX + 2.0);
    double u2 = (rand() + 1.0)/(RAND_MAX + 2.0);
    return sqrt(-2.0*log(u1))*cos(2.0*PI*u2);
}

// error = ||x - x_rec|| / ||x||
double get_error(const vector* x, const vector* x_rec)
{
    vector diff = vector_sub(x, x_rec);
    double err = vector_norm(&diff)/vector_norm(x);
    delete_vector(&diff);
    return err;
}

// solve (A^T A + lambda I) x = A^T b
vector tikhonov(const matrix* A, const vector* b, const double lambda)
{
    matrix At = matrix_transpose(A);
    matrix M = matrix_mult(&At, A);
    for (int i = 1; i <= M.rows; i++)
    { mget(M,i,i) = mget(M,i,i) + lambda; }
    vector Atb = matrix_vector_mult(&At, b);

    vector x = solve(&M, &Atb);

    delete_matrix(&At);
    delete_matrix(&M);
    delete_vector(&Atb);
    return x;
}

int main()
{
    int n = 128;
    double sigma[2] = {1.0e-4, 1.0e-2};
    double lambda[2] = {1.0e-4, 1.0e-2};

    srand(1);

    // blur matrix A
    matrix A = new_matrix(n, n);
    for (int i = 1; i <= n; i++)
    {
        mget(A,i,i) = 2.0/4.0;
        if (i > 1) { mget(A,i,i-1) = 1.0/4.0; }
        if (i < n) { mget(A,i,i+1) = 1.0/4.0; }
    }

    // true signal x
    vector x = new_vector(n);
    for (int i = 1; i <= n; i++)
    {
        if (i >= n/4 && i <= n/2) { vget(x,i) = 1.0; }
    }

    // blurred signal b = A x
    vector b = matrix_vector_mult(&A, &x);

    printf("\n n = %i\n", n);

    for (int s = 0; s < 2; s++)
    {
        // add noise to b
        vector b_noise = new_vector(n);
        for (int i = 1; i <= n; i++)
        { vget(b_noise,i) = vget(b,i) + sigma[s]*randn(); }

        printf("\n sigma = %.0e\n", sigma[s]);

        vector x_rec = solve(&A, &b_noise);
        printf("   no regularization: error = %e\n", get_error(&x, &x_rec));
        delete_vector(&x_rec);

        for (int l = 0; l < 2; l++)
        {
            vector x_tik = tikhonov(&A, &b_noise, lambda[l]);
            printf("   lambda = %.0e:     error = %e\n", lambda[l], get_error(&x, &x_tik));
            delete_vector(&x_tik);
        }

        delete_vector(&b_noise);
    }
    printf("\n");

    delete_matrix(&A);
    delete_vector(&x);
    delete_vector(&b);
    return 0;
}
