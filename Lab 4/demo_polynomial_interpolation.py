import math
import numpy as np

from gauss_elimination_solve import GaussElimination

x_points = [-0.1, -0.02, 0.02, 0.1]
y_points = [math.cos(x) for x in x_points]


A = []
for x in x_points:
    A.append([x**3, x**2, x, 1])
A = np.array(A)

b = np.array(y_points)

print('A and b are:')
print(A)
print(b)


coeffs = GaussElimination(A, b)
a = coeffs[0]
b_coef = coeffs[1]
c = coeffs[2]
d = coeffs[3]

print('Solved coefficients [a, b, c, d]:')
print(coeffs)

print('p(x) = %f*x^3 + %f*x^2 + %f*x + %f' % (a, b_coef, c, d))
