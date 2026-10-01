#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "node.h"

/* Lab 12: stack application - finding all roots of f(x) on [a,b]
   with the bisection method, using a stack of intervals */

int main()
{
    int choice, trace;
    double a, b, tol, h_min;

    printf("\n Bisection method with a stack\n");
    for (int i = 1; i <= NUM_FUNCTIONS; i++)
    {
        printf("   %i - %s\n", i, FunctionName(i));
    }

    printf(" Choose a function: ");
    if (scanf("%i", &choice) != 1 || choice < 1 || choice > NUM_FUNCTIONS)
    {
        printf("\n Error: choice must be 1 to %i.\n", NUM_FUNCTIONS);
        exit(1);
    }

    printf(" Enter interval a b: ");
    if (scanf("%lf %lf", &a, &b) != 2 || a >= b)
    {
        printf("\n Error: need two numbers with a < b.\n");
        exit(1);
    }

    printf(" Enter tolerance: ");
    if (scanf("%lf", &tol) != 1 || tol <= 0.0)
    {
        printf("\n Error: tolerance must be positive.\n");
        exit(1);
    }

    printf(" Enter search width h_min: ");
    if (scanf("%lf", &h_min) != 1 || h_min <= 0.0)
    {
        printf("\n Error: h_min must be positive.\n");
        exit(1);
    }

    printf(" Show step-by-step trace (1 = yes, 0 = no): ");
    if (scanf("%i", &trace) != 1)
    {
        printf("\n Error: trace must be 0 or 1.\n");
        exit(1);
    }

    printf("\n %s on [%g, %g], tol = %g, h_min = %g\n",
           FunctionName(choice), a, b, tol, h_min);

    double roots[MAX_ROOTS];
    int num_steps, max_stack;
    const int num_roots = Bisection(choice, a, b, tol, h_min, trace,
                                    roots, &num_steps, &max_stack);

    printf("\n Intervals popped:   %i\n", num_steps);
    printf(" Largest stack size: %i\n", max_stack);
    printf(" Roots found:        %i\n", num_roots);
    if (num_roots > 0)
    {
        printf("\n  -------------------------------------\n");
        printf("  | # |       x        |     f(x)      |\n");
        printf("  -------------------------------------\n");
        for (int i = 0; i < num_roots; i++)
        {
            printf("  |%2i | %14.10f | %13.4e |\n",
                   i + 1, roots[i], Function(choice, roots[i]));
        }
        printf("  -------------------------------------\n");
    }
    printf("\n");

    return 0;
}
