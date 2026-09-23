#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define E 2.718281828459045


long long myfactorial(int n)
{
    long long s = 1;
    for (int k = 1; k < n; k++)
    {
        s = s * (k + 1);
    }
    return s;
}

double myexp(double x)
{
    int terms = 30;
    int x0 = (int) round(x);
    double z = x - x0;

    double s = 0.0;
    double term = 1.0;
    for (int k = 0; k < terms; k++)
    {
        s = s + term;
        term = term * z / (k + 1);
    }

    
    double ex0 = 1.0;
    for (int k = 0; k < abs(x0); k++)
    {
        ex0 = ex0 * E;
    }
    if (x0 < 0)
    {
        ex0 = 1.0 / ex0;
    }

    return s * ex0;
}

double mylog(double x)
{
    int kmax = 100;
    
    double y = x;
    double s = 0.0;
    while (y > E)
    {
        y = y / E;
        s = s + 1;
    }
    while (y < 1.0 / E)
    {
        y = y * E;
        s = s - 1;
    }

    for (int k = 0; k < kmax; k++)
    {
        s = s - 1 + x * myexp(-s);
    }
    return s;
}


int main()
{
    int n;
    double x;

    // factorial
    printf("Enter a non-negative integer n: ");
    scanf("%d", &n);
    if (n < 0)
    {
        printf("n must be non-negative\n");
        return 1;
    }
    printf("myfactorial(%d) = %lld   expected %.0f\n", n, myfactorial(n), tgamma(n + 1));

    // exponent
    printf("\nEnter a real number x for exp: ");
    scanf("%lf", &x);
    printf("myexp(%g) = %.15f   expected %.15f\n", x, myexp(x), exp(x));

    // logarithm
    printf("\nEnter a positive real number x for log: ");
    scanf("%lf", &x);
    if (x <= 0)
    {
        printf("x must be positive\n");
        return 1;
    }
    printf("mylog(%g) = %.15f   expected %.15f\n", x, mylog(x), log(x));

    return 0;
}
