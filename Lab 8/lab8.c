#include <stdio.h>
#include <math.h>

#define E 2.718281828459

long long myfactorial(int n)
{
    if (n <= 1)
    {
        
        return 1;
    }
    else
    {
        return n * myfactorial(n - 1);
    }
}

double myexp(double x)
{
    int terms = 20;
    int x0 = (int) round(x);
    double z = x - x0;

    double s = 0.0;
    for (int k = 0; k < terms; k++)
    {
        s = s + pow(z, k) / myfactorial(k);
    }

    return s * pow(E, x0);
}

int main()
{
    int n;
    double a = 0.0, b = 1.0, h = 0.02;
    int np = (int)((b - a) / h) + 1;

    double x[51];
    double y[51];


    printf("Enter a non-negative integer n: ");
    scanf("%i", &n);
    if (n < 0)
    {
        printf("n must be non-negative\n");
        return 1;
    }
    printf("myfactorial(%i) = %lld\n", n, myfactorial(n));


    for (int i = 0; i < np; i++)
    {
        x[i] = a + i * h;
        y[i] = myexp(x[i]);
    }


    printf("\n     x            myexp(x)\n");
    for (int i = 0; i < np; i++)
    {
        printf("  %5.2f  %23.16e\n", x[i], y[i]);
    }

    
    FILE *outfile = fopen("exp.data", "w");

   
    for (int i = 0; i < np; i++)
    {
        fprintf(outfile, " %23.16e %23.16e\n", x[i], y[i]);
    }

    fclose(outfile);

    printf("\nData written to exp.data\n");

    return 0;
}
