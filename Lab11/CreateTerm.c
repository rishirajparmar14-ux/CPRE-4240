#include <stdio.h>
#include <stdlib.h>
#include "poly.h"

/* make a new node and fill it in */
term *CreateTerm(double coeff, int exp)
{
    term *t = (term *)malloc(sizeof(term));
    if (t == NULL)
    {
        printf("\n Error: out of memory.\n");
        exit(1);
    }

    t->coeff = coeff;
    t->exp = exp;
    t->next = NULL;

    return t;
}
