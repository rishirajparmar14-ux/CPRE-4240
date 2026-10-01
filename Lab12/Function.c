#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "node.h"

/* the functions the user can pick from in main */
double Function(const int choice, const double x)
{
    switch (choice)
    {
    case 1:
        return x * x * x - 3.0 * x + 1.0;
    case 2:
        return cos(x) - x;
    case 3:
        return sin(x) - x / 4.0;
    default:
        printf("\n Error: function choice must be 1 to %i.\n", NUM_FUNCTIONS);
        exit(1);
    }
}

const char *FunctionName(const int choice)
{
    switch (choice)
    {
    case 1:
        return "f(x) = x^3 - 3x + 1";
    case 2:
        return "f(x) = cos(x) - x";
    case 3:
        return "f(x) = sin(x) - x/4";
    default:
        return "unknown";
    }
}
