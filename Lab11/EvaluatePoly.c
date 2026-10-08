#include <stdlib.h>
#include "poly.h"

/* find the value of the polynomial at x
   by adding up coeff * x^exp for every term */
double EvaluatePoly(term *head, double x)
{
    double result = 0.0;

    term *ptr = head;
    while (ptr != NULL)
    {
        /* compute x^exp */
        double power = 1.0;
        for (int k = 0; k < ptr->exp; k++)
        {
            power = power * x;
        }

        result = result + ptr->coeff * power;
        ptr = ptr->next;
    }

    return result;
}
