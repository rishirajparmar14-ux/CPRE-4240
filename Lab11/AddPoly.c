#include <stdlib.h>
#include "poly.h"

/* make a new list S = P + Q.
   InsertTerm already combines terms with the same exponent,
   so we can just insert every term of P and then every term of Q. */
term *AddPoly(term *p, term *q)
{
    term *sum = NULL;

    term *ptr = p;
    while (ptr != NULL)
    {
        InsertTerm(&sum, ptr->coeff, ptr->exp);
        ptr = ptr->next;
    }

    ptr = q;
    while (ptr != NULL)
    {
        InsertTerm(&sum, ptr->coeff, ptr->exp);
        ptr = ptr->next;
    }

    return sum;
}
