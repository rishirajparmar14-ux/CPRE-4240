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
# We look for p(x) = c[0]*x^n + c[1]*x^(n-1) + ... + c[n-1]*x + c[n]
# and return the coefficients c (highest power first, same order as np.polyfit).
def LeastSquareApprox(x, f, n):
    x = np.array(x, dtype=float)
    f = np.array(f, dtype=float)

    m = len(x)  # number of data points

    # Build the m by (n+1) matrix V, one row per data point:
    # row i is [x_i^n, x_i^(n-1), ..., x_i, 1]
    V = np.zeros((m, n + 1))
    for i in range(m):
        for j in range(n + 1):
            V[i][j] = x[i] ** (n - j)

    # The least square solution satisfies the normal equations
    #   (V^T V) c = V^T f
    # which is a square (n+1) by (n+1) system, so we can reuse GaussElimination.
    A = np.dot(V.T, V)
    b = np.dot(V.T, f)

    c = GaussElimination(A, b)

    return c


# evaluate p(x) given the coefficients from LeastSquareApprox (highest power first)
def polyval(c, x):
    x = np.array(x, dtype=float)
    n = len(c) - 1

    p = np.zeros_like(x)
    for j in range(len(c)):
        p = p + c[j] * x ** (n - j)

    return p


# main program: approximate f(x) = cos(x) on linspace(-pi, pi, 51)
# by a polynomial of degree <= 5 in the least square sense
if __name__ == '__main__':
    x = np.linspace(-np.pi, np.pi, 51)
    f = np.cos(x)
    n = 5

    c = LeastSquareApprox(x, f, n)

    print('Least square coefficients (highest power first):')
    for j in range(len(c)):
        print('  x^%d : %12.8f' % (n - j, c[j]))

    print()
    print('p(x) = %.8f*x^5 + %.8f*x^4 + %.8f*x^3 + %.8f*x^2 + %.8f*x + %.8f'
          % (c[0], c[1], c[2], c[3], c[4], c[5]))

    # how good is the fit?
    p = polyval(c, x)
    error = f - p
    print()
    print('Largest error |f - p| at the nodes :', np.max(np.abs(error)))
    print('Least square error ||f - p||_2     :', np.sqrt(np.sum(error ** 2)))

    # plot f and p together. Use a finer grid for the curves so they look smooth.
    xs = np.linspace(-np.pi, np.pi, 400)

    plt.figure()
    plt.plot(xs, np.cos(xs), 'b-', label='f(x) = cos(x)')
    plt.plot(xs, polyval(c, xs), 'r--', label='p(x), least square, degree <= 5')
    plt.plot(x, f, 'k.', markersize=5, label='data nodes')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Least square approximation of cos(x) by a degree 5 polynomial')
    plt.legend()
    plt.grid(True)
    plt.savefig('least_square_cos.png', dpi=150)
    plt.show()
