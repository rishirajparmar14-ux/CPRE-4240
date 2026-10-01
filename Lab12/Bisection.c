#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "node.h"

/*
   INPUT:  choice - which f(x) to use (see Function.c)
           [a,b]  - interval to search
           tol    - stop bisecting a bracket once its width is below tol
           h_min  - intervals with no sign change are only split further
                    while wider than h_min (roots closer together than
                    h_min may be missed)
           trace  - 1 prints every pop/push, 0 prints nothing
   OUTPUT: roots[]   - the roots found, in increasing order
           num_steps - number of intervals popped off the stack
           max_stack - largest size the stack reached
           returns the number of roots found

   The classic bisection method keeps only one bracket [a,b] with
   f(a)*f(b) < 0. Here a stack holds all intervals that still need
   work, so every root on [a,b] is found, not just one:
     - pop an interval and look at its midpoint m
     - if it brackets a root and is narrower than tol, m is a root
     - otherwise push both halves (right half first, so the left half
       is popped next and roots come out from left to right)
     - intervals with no sign change are dropped once narrower than h_min
*/
int Bisection(const int choice, const double a, const double b,
              const double tol, const double h_min, const int trace,
              double roots[], int *num_steps, int *max_stack)
{
    node *top = NULL;
    int num_roots = 0;
    int stack_size = 0;
    double left, right;

    *num_steps = 0;
    *max_stack = 0;

    /* a root sitting exactly on an end point is never a midpoint */
    if (Function(choice, a) == 0.0)
    {
        roots[num_roots++] = a;
    }

    Push(a, b, &top);

    if (trace)
    {
        Peek(top, &left, &right);
        printf("\n Initial stack (top = [%g, %g]):\n", left, right);
        DisplayStack(top);
        printf("\n  Step |      Popped interval        | Left | Action\n");
        printf(" ------+-----------------------------+------+---------------------\n");
    }

    while (top != NULL)
    {
        GetStackSize(top, &stack_size);
        if (stack_size > *max_stack)
        {
            *max_stack = stack_size;
        }

        Pop(&top, &left, &right);
        *num_steps = *num_steps + 1;

        const double mid = 0.5 * (left + right);
        const double width = right - left;
        const double f_left = Function(choice, left);
        const double f_mid = Function(choice, mid);
        const double f_right = Function(choice, right);
        const int sign_change = (f_left * f_right < 0.0);

        GetStackSize(top, &stack_size);
        if (trace)
        {
            printf(" %5i | [%12.8f,%12.8f] | %4i | ",
                   *num_steps, left, right, stack_size);
        }

        if (f_mid == 0.0 || (sign_change && width < tol))
        {
            if (num_roots == MAX_ROOTS)
            {
                printf("\n Error: more than %i roots found.\n", MAX_ROOTS);
                DeleteStack(&top);
                exit(1);
            }
            roots[num_roots++] = mid;
            if (trace)
            {
                printf("ROOT  x = %.10f\n", mid);
            }
            if (f_mid == 0.0 && width > h_min)
            {
                Push(mid, right, &top);
                Push(left, mid, &top);
            }
        }
        else if (sign_change || width > h_min)
        {
            Push(mid, right, &top);
            Push(left, mid, &top);
            if (trace)
            {
                printf("split, push 2 halves\n");
            }
        }
        else if (trace)
        {
            printf("no sign change, drop\n");
        }
    }

    if (Function(choice, b) == 0.0 && a != b)
    {
        roots[num_roots++] = b;
    }

    DeleteStack(&top);

    /* a root found exactly at a midpoint is recorded before the roots
       to its left, so sort them (insertion sort, the list is short) */
    for (int i = 1; i < num_roots; i++)
    {
        const double x = roots[i];
        int j = i - 1;
        while (j >= 0 && roots[j] > x)
        {
            roots[j + 1] = roots[j];
            j--;
        }
        roots[j + 1] = x;
    }

    return num_roots;
}
