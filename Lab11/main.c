#include <stdio.h>
#include <stdlib.h>
#include "poly.h"

int main()
{
    term *P = NULL;
    term *Q = NULL;

    /* read the two polynomials (terms can be typed in any order) */
    ReadPoly(&P, "P");
    ReadPoly(&Q, "Q");

    /* add them */
    term *S = AddPoly(P, Q);

    /* print the polynomials and their linked lists */
    printf("\n");
    PrintPoly("P", P);
    PrintTable(P);

    PrintPoly("Q", Q);
    PrintTable(Q);

    PrintPoly("S", S);
    PrintTable(S);

    /* evaluate at a value of x */
    double x;
    printf(" Enter a value for x: ");
    if (scanf("%lf", &x) != 1)
    {
        printf("\n Error: x must be a number.\n");
        exit(1);
    }

    double p_val = EvaluatePoly(P, x);
    double q_val = EvaluatePoly(Q, x);
    double s_val = EvaluatePoly(S, x);

    printf("\n P(%g) = %g\n", x, p_val);
    printf(" Q(%g) = %g\n", x, q_val);
    printf(" S(%g) = %g\n\n", x, s_val);

    /* free the memory */
    DeletePoly(&P);
    DeletePoly(&Q);
    DeletePoly(&S);

    return 0;
}
