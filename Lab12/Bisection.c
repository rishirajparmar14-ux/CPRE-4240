#include <stdio.h>
#include <stdlib.h>
#include "node.h"

// Finds all the roots of f on [a,b] using bisection.
// The stack holds the intervals we still need to check. We pop one,
// and if it has a sign change and is smaller than tol we take the
// midpoint as a root. Otherwise we split it in half and push both halves.
// Intervals with no sign change are thrown away once they are smaller
// than h_min.
int Bisection(const int choice, const double a, const double b,
              const double tol, const double h_min, const int show,
              double roots[], int* steps)
{
    node* top = NULL;
    int num_roots = 0;
    int size;
    double left, right;
    *steps = 0;

    Push(a, b, &top);

    if (show == 1)
    {
        Peek(top, &left, &right);
        printf("\n Starting stack (top is [%g, %g]):\n", left, right);
        DisplayStack(top);
        printf("\n");
    }

    while (top != NULL)
    {
        Pop(&top, &left, &right);
        *steps = *steps + 1;

        double mid = (left + right)/2.0;
        double width = right - left;
        int sign_change = (f(choice, left)*f(choice, right) < 0.0);

        if (f(choice, mid) == 0.0 || (sign_change && width < tol))
        {
            roots[num_roots] = mid;
            num_roots++;
            if (show == 1)
            { printf(" Step %i: [%f, %f] root found at x = %f\n", *steps, left, right, mid); }

            // if mid was exactly a root, the halves could still have more roots
            if (f(choice, mid) == 0.0 && width > h_min)
            {
                Push(mid, right, &top);
                Push(left, mid, &top);
            }
        }
        else if (sign_change || width > h_min)
        {
            // push right half first so the left half gets checked first
            Push(mid, right, &top);
            Push(left, mid, &top);
            if (show == 1)
            {
                GetStackSize(top, &size);
                printf(" Step %i: [%f, %f] split, stack size = %i\n", *steps, left, right, size);
            }
        }
        else
        {
            if (show == 1)
            { printf(" Step %i: [%f, %f] no root, removed\n", *steps, left, right); }
        }
    }

    DeleteStack(&top);

    // sort the roots from smallest to largest
    for (int i = 0; i < num_roots - 1; i++)
    {
        for (int j = 0; j < num_roots - 1 - i; j++)
        {
            if (roots[j] > roots[j+1])
            {
                double temp = roots[j];
                roots[j] = roots[j+1];
                roots[j+1] = temp;
            }
        }
    }

    return num_roots;
}
