#include <stdio.h>
#include "poly.h"

/* print the polynomial like  P(x) = 3x^4 - 2x^2 + x - 5 */
void PrintPoly(char *name, term *head)
{
    printf(" %s(x) = ", name);

    if (head == NULL)
    {
        printf("0\n");
        return;
    }

    term *ptr = head;
    while (ptr != NULL)
    {
        double c = ptr->coeff;

        /* print the sign */
        if (ptr == head)
        {
            if (c < 0)
            {
                printf("-");
            }
        }
        else
        {
            if (c < 0)
            {
                printf(" - ");
            }
            else
            {
                printf(" + ");
            }
        }

        if (c < 0)
        {
            c = -c;
        }

        /* don't print a 1 in front of x (but do for the constant) */
        if (c != 1.0 || ptr->exp == 0)
        {
            printf("%g", c);
        }

        if (ptr->exp == 1)
        {
            printf("x");
        }
        else if (ptr->exp > 1)
        {
            printf("x^%i", ptr->exp);
        }

        ptr = ptr->next;
    }
    printf("\n");
}

/* print each node of the list with its address and next pointer */
void PrintTable(term *head)
{
    if (head == NULL)
    {
        printf("   (empty list)\n\n");
        return;
    }

    printf("      coeff  exp   address            next\n");

    term *ptr = head;
    while (ptr != NULL)
    {
        printf("   %8.3f  %3i   %p   %p\n",
               ptr->coeff, ptr->exp, (void *)ptr, (void *)ptr->next);
        ptr = ptr->next;
    }
    printf("\n");
}
