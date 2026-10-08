#ifndef __POLY_H__
#define __POLY_H__

/* one term of a polynomial: coeff * x^exp
   the list is kept in order from highest to lowest exponent */
typedef struct term term;
struct term
{
    double coeff;
    int exp;
    term *next;
};

term *CreateTerm(double coeff, int exp);
void InsertTerm(term **head, double coeff, int exp);
void ReadPoly(term **head, char *name);
void PrintPoly(char *name, term *head);
void PrintTable(term *head);
term *AddPoly(term *p, term *q);
double EvaluatePoly(term *head, double x);
void DeletePoly(term **head);

#endif
