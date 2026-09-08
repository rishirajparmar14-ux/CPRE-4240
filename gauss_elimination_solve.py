import numpy as np


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
