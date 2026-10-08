#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "eigen.h"

// Power iteration: finds the largest eigenvalue
// v is the starting guess, and holds the eigenvector at the end
double power_iteration(const matrix* A, vector* v, double TOL, int MaxIters)
{
    int n = v->size;

    // v = v/||v||
    double norm = vector_norm(v);
    for (int i=1; i<=n; i++) { vgetp(v,i) = vgetp(v,i)/norm; }

    // lambda = v^T A v
    vector Av = matrix_vector_mult(A,v);
    double lambda = vector_dot_mult(v,&Av);
    delete_vector(&Av);

    int k = 0;
    int mstop = 0;
    while (mstop==0)
    {
        k = k+1;

        // w = A v,  v = w/||w||
        vector w = matrix_vector_mult(A,v);
        norm = vector_norm(&w);
        for (int i=1; i<=n; i++) { vgetp(v,i) = vget(w,i)/norm; }
        delete_vector(&w);

        // lambda = v^T A v
        double lambda_old = lambda;
        Av = matrix_vector_mult(A,v);
        lambda = vector_dot_mult(v,&Av);
        delete_vector(&Av);

        if (fabs(lambda-lambda_old) < TOL || k==MaxIters) { mstop = 1; }
    }

    printf("   iterations = %i\n", k);
    return lambda;
}

// Shifted inverse power iteration: finds the eigenvalue closest to mu
double shifted_inverse_iteration(const matrix* A, vector* v, double mu,
                                 double TOL, int MaxIters)
{
    int n = v->size;

    // B = A - mu*I
    matrix B = new_matrix(n,n);
    for (int i=1; i<=n; i++)
    for (int j=1; j<=n; j++)
    { mget(B,i,j) = mgetp(A,i,j); }
    for (int i=1; i<=n; i++) { mget(B,i,i) = mget(B,i,i) - mu; }

    // v = v/||v||
    double norm = vector_norm(v);
    for (int i=1; i<=n; i++) { vgetp(v,i) = vgetp(v,i)/norm; }

    // lambda = v^T A v
    vector Av = matrix_vector_mult(A,v);
    double lambda = vector_dot_mult(v,&Av);
    delete_vector(&Av);

    int k = 0;
    int mstop = 0;
    while (mstop==0)
    {
        k = k+1;

        // solve (A - mu*I) w = v,  v = w/||w||
        vector w = solve(&B,v);
        norm = vector_norm(&w);
        for (int i=1; i<=n; i++) { vgetp(v,i) = vget(w,i)/norm; }
        delete_vector(&w);

        // lambda = v^T A v
        double lambda_old = lambda;
        Av = matrix_vector_mult(A,v);
        lambda = vector_dot_mult(v,&Av);
        delete_vector(&Av);

        if (fabs(lambda-lambda_old) < TOL || k==MaxIters) { mstop = 1; }
    }

    printf("   iterations = %i\n", k);
    delete_matrix(&B);
    return lambda;
}

// Rayleigh quotient iteration: same as shifted inverse iteration,
// but the shift is updated with lambda every iteration
double rayleigh_quotient_iteration(const matrix* A, vector* v,
                                   double TOL, int MaxIters)
{
    int n = v->size;

    // v = v/||v||
    double norm = vector_norm(v);
    for (int i=1; i<=n; i++) { vgetp(v,i) = vgetp(v,i)/norm; }

    // lambda = v^T A v
    vector Av = matrix_vector_mult(A,v);
    double lambda = vector_dot_mult(v,&Av);
    delete_vector(&Av);

    int k = 0;
    int mstop = 0;
    while (mstop==0)
    {
        k = k+1;

        // B = A - lambda*I
        matrix B = new_matrix(n,n);
        for (int i=1; i<=n; i++)
        for (int j=1; j<=n; j++)
        { mget(B,i,j) = mgetp(A,i,j); }
        for (int i=1; i<=n; i++) { mget(B,i,i) = mget(B,i,i) - lambda; }

        // solve (A - lambda*I) w = v,  v = w/||w||
        vector w = solve(&B,v);
        norm = vector_norm(&w);
        for (int i=1; i<=n; i++) { vgetp(v,i) = vget(w,i)/norm; }
        delete_vector(&w);
        delete_matrix(&B);

        // lambda = v^T A v
        double lambda_old = lambda;
        Av = matrix_vector_mult(A,v);
        lambda = vector_dot_mult(v,&Av);
        delete_vector(&Av);

        if (fabs(lambda-lambda_old) < TOL || k==MaxIters) { mstop = 1; }
    }

    printf("   iterations = %i\n", k);
    return lambda;
}
