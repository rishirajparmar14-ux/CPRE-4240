#include <stdio.h>
#include <stdlib.h>

#define NMAX 5
#define NPTS 101


double chebyshev(int i, double x)
{
    double phi;

    switch (i)
    {
    case 0:
        phi = 1.0;
        break;
    case 1:
        phi = x;
        break;
    case 2:
        phi = 2.0 * x * x - 1.0;
        break;
    case 3:
        phi = 4.0 * x * x * x - 3.0 * x;
        break;
    case 4:
        phi = 8.0 * x * x * x * x - 8.0 * x * x + 1.0;
        break;
    case 5:
        phi = 16.0 * x * x * x * x * x - 20.0 * x * x * x + 5.0 * x;
        break;
    default:
        printf("chebyshev: i = %i is out of range, returning 0\n", i);
        phi = 0.0;
        break;
    }

    return phi;
}


double expansion(int N, double b[], double x)
{
    double s = 0.0;
    for (int i = 0; i <= N; i++)
    {
        s = s + b[i] * chebyshev(i, x);
    }
    return s;
}


int main()
{
    int N;
    double b[NMAX + 1];
    double a = -1.0, c = 1.0;
    double h = (c - a) / (NPTS - 1);

    double x[NPTS];
    double p[NPTS];


    printf("Enter the degree N (0 <= N <= %i): ", NMAX);
    scanf("%i", &N);
    if (N < 0 || N > NMAX)
    {
        printf("N must be between 0 and %i\n", NMAX);
        return 1;
    }

    for (int i = 0; i <= N; i++)
    {
        printf("Enter b[%i]: ", i);
        scanf("%lf", &b[i]);
    }


    for (int i = 0; i < NPTS; i++)
    {
        x[i] = a + i * h;
        p[i] = expansion(N, b, x[i]);
    }


    printf("\n     x                    p_%i(x)\n", N);
    for (int i = 0; i < NPTS; i++)
    {
        printf("  %5.2f  %23.16e\n", x[i], p[i]);
    }


    FILE *outfile = fopen("chebyshev.data", "w");
    if (outfile == NULL)
    {
        printf("could not open chebyshev.data\n");
        return 1;
    }

    fprintf(outfile, "# N = %i\n", N);
    fprintf(outfile, "# b =");
    for (int i = 0; i <= N; i++)
    {
        fprintf(outfile, " %g", b[i]);
    }
    fprintf(outfile, "\n");

    for (int i = 0; i < NPTS; i++)
    {
        fprintf(outfile, " %23.16e %23.16e\n", x[i], p[i]);
    }

    fclose(outfile);

    printf("\nData written to chebyshev.data\n");


    printf("Running the Python script to plot the polynomial...\n");
    int status = system("python plot_chebyshev.py");
    if (status != 0)
    {
        printf("the Python script did not run successfully\n");
        return 1;
    }

    return 0;
}
