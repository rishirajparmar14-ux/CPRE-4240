#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "eigen.h"

#define PI 3.14159265358979323846

// Lab 14 - eigenvalue solvers for the 4-DOF mass-spring system K x = lambda x

// print lambda, omega and the mode shape
void print_result(double lambda, vector* v)
{
    printf("   lambda     = %f\n", lambda);
    printf("   omega      = %f\n", sqrt(lambda));
    printf("   mode shape = ");
    for (int i=1; i<=v->size; i++) { printf("%10.6f", vgetp(v,i)); }
    printf("\n\n");
}

int main()
{
    int n = 4;
    double TOL = 1.0e-12;
    int MaxIters = 1000;

    // stiffness matrix K
    matrix K = new_matrix(n,n);
    for (int i=1; i<=n; i++)
    {
        mget(K,i,i) = 2.0;
        if (i>1) { mget(K,i,i-1) = -1.0; }
        if (i<n) { mget(K,i,i+1) = -1.0; }
    }

    printf("\n Stiffness matrix K:\n");
    for (int i=1; i<=n; i++)
    {
        printf("   ");
        for (int j=1; j<=n; j++) { printf("%6.1f", mget(K,i,j)); }
        printf("\n");
    }
    printf("\n");

    vector v = new_vector(n);
    double lambda;

    // Power iteration
    printf(" Power iteration\n");
    vget(v,1) = 1.0; vget(v,2) = 0.0; vget(v,3) = 0.0; vget(v,4) = 0.0;
    lambda = power_iteration(&K,&v,TOL,MaxIters);
    print_result(lambda,&v);

    // Shifted inverse power iteration with different shifts
    double mu[4] = {0.0, 1.2, 2.8, 3.5};
    for (int s=0; s<4; s++)
    {
        printf(" Shifted inverse power iteration (mu = %.1f)\n", mu[s]);
        vget(v,1) = 1.0; vget(v,2) = 0.0; vget(v,3) = 0.0; vget(v,4) = 0.0;
        lambda = shifted_inverse_iteration(&K,&v,mu[s],TOL,MaxIters);
        print_result(lambda,&v);
    }

    // Rayleigh quotient iteration with different starting vectors
    double v0[4][4] = { {1.0,  2.0,  2.0,  1.0},
                        {1.0,  1.0, -1.0, -1.0},
                        {1.0, -1.0, -1.0,  1.0},
                        {1.0, -2.0,  2.0, -1.0} };
    for (int s=0; s<4; s++)
    {
        printf(" Rayleigh quotient iteration (start vector %i)\n", s+1);
        for (int i=1; i<=n; i++) { vget(v,i) = v0[s][i-1]; }
        lambda = rayleigh_quotient_iteration(&K,&v,TOL,MaxIters);
        print_result(lambda,&v);
    }

    // exact eigenvalues: lambda_j = 2 - 2cos(j*pi/5)
    printf(" Exact eigenvalues\n");
    for (int j=1; j<=n; j++)
    { printf("   lambda_%i = %f\n", j, 2.0 - 2.0*cos(j*PI/5.0)); }
    printf("\n");

    delete_vector(&v);
    delete_matrix(&K);
    return 0;
}
