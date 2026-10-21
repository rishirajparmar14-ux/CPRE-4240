#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "trimatrix.h"

// Lab 15 - QR algorithm on the matrix K from Lab 14

void Hessenberg(const matrix* A, trimatrix* T);
void QRA(trimatrix* T);

// print a value, set tiny numbers to 0 so -0.000000 does not show up
void print_value(double x)
{
    if (fabs(x) < 1.0e-12) { x = 0.0; }
    printf("%10.6f", x);
}

void run_qr(int N)
{
    // build K: 2 on the diagonal, -1 next to it
    matrix K = new_matrix(N,N);
    for (int i=1; i<=N; i++)
    {
        mget(K,i,i) = 2.0;
        if (i>1) { mget(K,i,i-1) = -1.0; }
        if (i<N) { mget(K,i,i+1) = -1.0; }
    }

    printf("N = %d\n\n", N);
    printf("Original matrix:\n");
    for (int i=1; i<=N; i++)
    {
        for (int j=1; j<=N; j++) { print_value(mget(K,i,j)); }
        printf("\n");
    }

    // Phase I: reduce to tridiagonal
    trimatrix T = new_trimatrix(N);
    Hessenberg(&K,&T);

    printf("\nTridiagonal matrix after Phase I:\n");
    for (int i=1; i<=N; i++)
    {
        for (int j=1; j<=N; j++) { print_value(tget(&T,i,j)); }
        printf("\n");
    }

    // Phase II: QR algorithm
    QRA(&T);

    printf("\nDiagonal matrix after Phase II:\n");
    for (int i=1; i<=N; i++)
    {
        for (int j=1; j<=N; j++) { print_value(tget(&T,i,j)); }
        printf("\n");
    }
    printf("\n");

    delete_trimatrix(&T);
    delete_matrix(&K);
}

int main()
{
    run_qr(6);
    run_qr(8);
    return 0;
}
