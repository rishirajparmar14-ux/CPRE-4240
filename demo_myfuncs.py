
import math


def mysqrt(x, kmax=50):
    
    s = 1.0
    for k in range(kmax):
        s = 0.5 * (s + x / s)
    return s


def myfactorial(n):
    
    s = 1
    for k in range(1, n):
        s = s * (k + 1)
    return s


def myexp(x, terms=30):
    e = math.e
    x0 = int(round(x))
    z = x - x0

    s = 0.0
    term = 1.0
    for k in range(terms):
        s += term
        term *= z / (k + 1)

    return s * e**x0


def mylog(x, kmax=100):
    s = x
    for k in range(kmax):
        s = s - 1 + x * math.exp(-s)
    return s


print("mysqrt(16) =", mysqrt(16), "expected", math.sqrt(16))
print("myfactorial(5) =", myfactorial(5), "expected", math.factorial(5))
print("myexp(2) =", myexp(2), "expected", math.exp(2))
print("mylog(10) =", mylog(10), "expected", math.log(10))
