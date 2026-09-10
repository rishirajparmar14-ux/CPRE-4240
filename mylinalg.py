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
# p(x) = c[0]*x^n + c[1]*x^(n-1) + ... + c[n]
# returns the coefficients c, highest power first (same order as np.polyfit)
def LeastSquareApprox(x, f, n):
    f = np.array(f, dtype=float)

    # one row per data point, same idea as the interpolation demo but now
    # there are more rows than unknowns
    V = []
    for xi in x:
        row = []
        for j in range(n + 1):
            row.append(xi ** (n - j))
        V.append(row)
    V = np.array(V)

    # this system is not square so we cannot solve it directly.
    # the least square solution comes from the normal equations
    #   (V^T V) c = V^T f
    # and that one is square, so GaussElimination works on it
    A = np.dot(V.T, V)
    b = np.dot(V.T, f)

    c = GaussElimination(A, b)

    return c


# evaluate p at x using the coefficients from LeastSquareApprox
def polyval(c, x):
    n = len(c) - 1

    p = 0.0
    for j in range(n + 1):
        p = p + c[j] * x ** (n - j)

    return p


# main program: approximate f(x) = cos(x) at the nodes linspace(-pi, pi, 51)
# by a polynomial of degree <= 5 in the least square sense
if __name__ == '__main__':
    n = 5
    x = np.linspace(-np.pi, np.pi, 51)
    f = np.cos(x)

    c = LeastSquareApprox(x, f, n)

    print('Coefficients (highest power first):')
    print(c)

    print('p(x) = %f*x^5 + %f*x^4 + %f*x^3 + %f*x^2 + %f*x + %f'
          % (c[0], c[1], c[2], c[3], c[4], c[5]))

    # the x^5, x^3 and x^1 coefficients come out as basically zero,
    # which makes sense because cos is an even function

    error = f - polyval(c, x)
    print('Biggest error at the nodes:', np.max(np.abs(error)))

    # plot f and p. Use more points than the 51 nodes so the curves look smooth.
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
