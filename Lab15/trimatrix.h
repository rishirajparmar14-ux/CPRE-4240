#ifndef __TRIMATRIX_H__
#define __TRIMATRIX_H__

#include "matrix.h"

typedef struct trimatrix trimatrix;
struct trimatrix
{
   int rows; int cols;
   double* left;
   double* center;
   double* right;
};

typedef struct pentamatrix pentamatrix;
struct pentamatrix
{
   int rows; int cols;
   double* leftleft;
   double* left;
   double* center;
   double* right;
   double* rightright;
};

#define print_trimatrix(mat) print_trimatrix_full(mat,#mat);
#define print_pentamatrix(mat) print_pentamatrix_full(mat,#mat);

// Function declarations
trimatrix new_trimatrix(const int rows);
void delete_trimatrix(trimatrix* t);
void print_trimatrix_full(const trimatrix* trimat, char* varname);
double tget(const trimatrix* trimat, const int i, const int j);
void tset(trimatrix* trimat, const int i, const int j, const double val);
trimatrix trimatrix_add(const trimatrix* A, const trimatrix* B);
trimatrix trimatrix_sub(const trimatrix* A, const trimatrix* B);
trimatrix trimatrix_dot_mult(const trimatrix* A, const trimatrix* B);
vector trimatrix_vector_mult(const trimatrix* A, const vector* x);
vector trisolve(const trimatrix* A, const vector* b);
//
pentamatrix new_pentamatrix(const int rows);
void delete_pentamatrix(pentamatrix* p);
void print_pentamatrix_full(const pentamatrix* p, char* varname);
double pentaget(const pentamatrix* p, const int i, const int j);
void pentaset(pentamatrix* p, const int i, const int j, const double val);
//
trimatrix grab_sub_trimatrix(trimatrix* T, int k1, int k2);
trimatrix merge_sub_trimatrix(trimatrix* T1, trimatrix* T2);
trimatrix matrix_mult_to_trimatrix(const matrix* A, const matrix* B);

#endif
