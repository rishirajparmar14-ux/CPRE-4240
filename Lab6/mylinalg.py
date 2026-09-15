# mylinalg.py
# Small linear algebra module for CPRE 4240.
# Contains GaussElimination (from the last assignment) and LeastSquareApprox.
# Running this file as a script fits cos(x) with a degree 5 polynomial.

import numpy as np
import matplotlib.pyplot as plt


# Gauss elimination (no partial pivoting) to solve Ax = b
# A is n by n, b is n by 1, returns x
def GaussElimination(A, b):
    A = np.array(A, dtype=float)  # make a copy so we don't mess up the original A
    b = np.array(b, dtype=float)  # same for b

    n = len(b)

    # forward elimination, same steps as in class
    for col in range(n - 1):
        for row in range(col + 1, n):
            m = A[row][col] / A[col][col]
            A[row][col:] = A[row][col:] - m * A[col][col:]
            b[row] = b[row] - m * b[col]

    # back substitution
    x = np.zeros(n)
    x[n - 1] = b[n - 1] / A[n - 1][n - 1]
    for row in range(n - 2, -1, -1):
        x[row] = (b[row] - np.dot(A[row][row + 1:], x[row + 1:])) / A[row][row]

    return x


# Least square approximation of the data {x, f} by a polynomial of degree <= n.
#
# We are looking for
#     p(x) = c[0]*x^n + c[1]*x^(n-1) + ... + c[n-1]*x + c[n]
# so there are n+1 unknown coefficients.
# The function returns the list of coefficients c, highest power first
# (this is the same order that np.polyfit uses).
def LeastSquareApprox(x, f, n):
    m = len(x)  # number of data points

    # Step 1: build the matrix V.
    # If we wanted p to pass exactly through every point we would need
    #     V * c = f
    # where row i of V is [x_i^n, x_i^(n-1), ..., x_i, 1].
    # This is the same matrix as in the interpolation demo, except now there
    # are m rows and only n+1 columns, so usually m > n+1 and the system has
    # no exact solution. That is why we do least squares instead.
    V = []
    for i in range(m):
        row = []
        for j in range(n + 1):
            power = n - j          # column j holds x to this power
            row.append(x[i] ** power)
        V.append(row)

    # Step 2: build the normal equations.
    # The least square solution is the solution of
    #     (V^T V) c = (V^T f)
    # Call the left matrix A and the right side rhs. Both are written out with
    # loops here so it is easy to see that each entry is just a sum over all
    # of the data points.
    A = np.zeros((n + 1, n + 1))
    for i in range(n + 1):
        for j in range(n + 1):
            total = 0.0
            for k in range(m):
                total = total + V[k][i] * V[k][j]   # column i dotted with column j
            A[i][j] = total

    rhs = np.zeros(n + 1)
    for i in range(n + 1):
        total = 0.0
        for k in range(m):
            total = total + V[k][i] * f[k]          # column i dotted with f
        rhs[i] = total

    # Step 3: A is square (n+1 by n+1), so we can solve it with the
    # Gauss elimination function from the last assignment.
    c = GaussElimination(A, rhs)

    return c


# Evaluate p(x) using the coefficients c that LeastSquareApprox returned.
# x can be a single number or a numpy array of numbers.
def polyval(c, x):
    n = len(c) - 1  # c has n+1 entries, so the highest power is n

    p = 0.0
    for j in range(n + 1):
        power = n - j
        p = p + c[j] * x ** power

    return p


# main program
# Approximate f(x) = cos(x) at the nodes linspace(-pi, pi, 51) by a polynomial
# of degree <= 5 in the least square sense, then plot f and p together.
if __name__ == '__main__':
    n = 5                                  # degree of the polynomial we want
    x = np.linspace(-np.pi, np.pi, 51)     # the 51 nodes
    f = np.cos(x)                          # the data values at those nodes

    c = LeastSquareApprox(x, f, n)

    print('Coefficients (highest power first):')
    print(c)

    print()
    print('p(x) = %f*x^5 + %f*x^4 + %f*x^3 + %f*x^2 + %f*x + %f'
          % (c[0], c[1], c[2], c[3], c[4], c[5]))

    # The x^5, x^3 and x^1 coefficients come out as basically zero.
    # That makes sense because cos is an even function, so the odd powers
    # are not needed to fit it.

    # Plot both curves.
    # We plot on 400 points instead of the 51 nodes so the curves look smooth.
    xs = np.linspace(-np.pi, np.pi, 400)

    plt.plot(xs, np.cos(xs), 'b-', label='f(x) = cos(x)')
    plt.plot(xs, polyval(c, xs), 'r--', label='p(x), degree 5 least square')
    plt.plot(x, f, 'k.', label='nodes')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Least square approximation of cos(x)')
    plt.legend()
    plt.grid(True)
    plt.savefig('least_square_cos.png')
    plt.show()
