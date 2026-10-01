#include <math.h>
#include "node.h"

double f(const int choice, const double x)
{
    if (choice == 1)
    { return x*x*x - 3.0*x + 1.0; }
    else if (choice == 2)
    { return cos(x) - x; }
    else
    { return sin(x) - x/4.0; }
}
