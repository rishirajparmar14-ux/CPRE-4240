#include <stdlib.h>
#include "poly.h"

/* put a term into the list in the right spot so the exponents
   go from highest to lowest. if the exponent is already in the
   list, just add the coefficients together. */
void InsertTerm(term **head, double coeff, int exp)
{
    if (coeff == 0.0)
    {
        return;
    }

    term *prev = NULL;
    term *cur = *head;

    /* move past all the terms with a bigger exponent */
    while (cur != NULL && cur->exp > exp)
    {
        prev = cur;
        cur = cur->next;
    }

    /* same exponent already there, so combine them */
    if (cur != NULL && cur->exp == exp)
    {
        cur->coeff = cur->coeff + coeff;

        /* if they cancel out, take the node out of the list */
        if (cur->coeff == 0.0)
        {
            if (prev == NULL)
            {
                *head = cur->next;
            }
            else
            {
                prev->next = cur->next;
            }
            free(cur);
        }
        return;
    }

    /* otherwise make a new node and link it in between prev and cur */
    term *t = CreateTerm(coeff, exp);
    t->next = cur;
    if (prev == NULL)
    {
        *head = t;
    }
    else
    {
        prev->next = t;
    }
}
