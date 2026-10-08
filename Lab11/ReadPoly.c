#include <stdio.h>
#include <stdlib.h>
#include "poly.h"

/* ask the user for the terms of a polynomial */
void ReadPoly(term **head, char *name)
{
    int n;

    printf("\n Enter the number of terms in %s(x): ", name);
    if (scanf("%i", &n) != 1 || n < 0)
    {
        printf("\n Error: bad number of terms\n");
        exit(1);
    }

    for (int i = 0; i < n; i++)
    {
        double coeff;
        int exp;

        printf("   term %i (coeff exp): ", i + 1);
        if (scanf("%lf %i", &coeff, &exp) != 2 || exp < 0)
        {
            printf("\n Error: bad term\n");
            exit(1);
        }

        InsertTerm(head, coeff, exp);
    }
}
