#ifndef __EIGEN_H__
#define __EIGEN_H__

#include "matrix.h"

double power_iteration(const matrix* A, vector* v, double TOL, int MaxIters);
double shifted_inverse_iteration(const matrix* A, vector* v, double mu,
                                 double TOL, int MaxIters);
double rayleigh_quotient_iteration(const matrix* A, vector* v,
                                   double TOL, int MaxIters);

#endif
