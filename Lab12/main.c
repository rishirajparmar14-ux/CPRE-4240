#include <stdio.h>
#include <stdlib.h>
#include "node.h"

// Lab 12 - Stack application to the Bisection method

int main()
{
    int choice, show, steps;
    double a, b, tol, h_min;
    double roots[100];

    printf("\n Functions:\n");
    printf(" 1 - f(x) = x^3 - 3x + 1\n");
    printf(" 2 - f(x) = cos(x) - x\n");
    printf(" 3 - f(x) = sin(x) - x/4\n");
    printf(" Pick a function: ");
    scanf("%i", &choice);
    if (choice < 1 || choice > 3)
    { printf(" Error: pick 1, 2 or 3.\n"); exit(1); }

    printf(" Enter a and b: ");
    scanf("%lf %lf", &a, &b);
    if (a >= b)
    { printf(" Error: a must be less than b.\n"); exit(1); }

    printf(" Enter tolerance: ");
    scanf("%lf", &tol);

    printf(" Enter h_min: ");
    scanf("%lf", &h_min);

    printf(" Show steps? (1 = yes, 0 = no): ");
    scanf("%i", &show);

    int num_roots = Bisection(choice, a, b, tol, h_min, show, roots, &steps);

    printf("\n Number of steps: %i\n", steps);
    printf(" Number of roots found: %i\n", num_roots);
    for (int i = 0; i < num_roots; i++)
    {
        printf(" Root %i: x = %.10f   f(x) = %e\n", i+1, roots[i], f(choice, roots[i]));
    }
    printf("\n");

    return 0;
}
