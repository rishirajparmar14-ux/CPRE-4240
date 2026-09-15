
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



# Part 1: exact formula
def logistic_exact(t_values, r, K, P0):
    A = (K - P0) / P0  # this constant comes from plugging in P(0) = P0

    P_values = []  # we will fill this list with one P(t) for each t
    for t in t_values:
        P = K / (1 + A * math.exp(-r * t))
        P_values.append(P)

    return P_values


# Part 2: forward Euler method
def logistic_euler(r, K, P0, t0, tf, dt):
    t_values = [t0]
    P_values = [P0]

    t = t0
    P = P0
    while t < tf:
        slope = r * P * (1 - P / K)  # this is dP/dt at the current point
        P = P + slope * dt           
        t = t + dt

        t_values.append(t)
        P_values.append(P)

    return t_values, P_values


# find the exact time when P(t) = K/2, using algebra on the formula above.
# Setting P(t) = K/2 and solving for t gives: t = ln(A) / r
def time_to_half_exact(r, K, P0):
    A = (K - P0) / P0
    return math.log(A) / r


# Part 3b: find (approximately) when P crosses K/2 inside a list of Euler results,
# by looking for the first step where P goes from below K/2 to above K/2.
def time_to_half_from_series(t_values, P_values, K):
    half = K / 2

    for i in range(1, len(P_values)):
        P_before = P_values[i - 1]
        P_after = P_values[i]

        crossed_upward = P_before < half and P_after >= half
        if crossed_upward:
            t_before = t_values[i - 1]
            t_after = t_values[i]
            
            fraction = (half - P_before) / (P_after - P_before)
            return t_before + fraction * (t_after - t_before)

    return None  


r = 0.5
K = 100
P0 = 10
t0 = 0
tf = 20
dt = 0.1

# 1. Evaluate the exact formula at a list of times: t = 0, 1, 2, ..., 20
sample_times = list(range(t0, tf + 1))
exact_values = logistic_exact(sample_times, r, K, P0)
print()
print("t values:", sample_times)
print("Exact P(t):", exact_values)

# 2. Compare the Euler approximation to the exact formula
t_euler, P_euler_approx = logistic_euler(r, K, P0, t0, tf, dt)
P_euler_exact = logistic_exact(t_euler, r, K, P0)

biggest_difference = 0
for i in range(len(P_euler_approx)):
    difference = abs(P_euler_approx[i] - P_euler_exact[i])
    if difference > biggest_difference:
        biggest_difference = difference

print()
print("Forward Euler with dt =", dt)
print("Largest difference between Euler and exact solution:", biggest_difference)

# 3. Find the time when the population reaches K/2 (half the carrying capacity)
t_half_exact = time_to_half_exact(r, K, P0)
t_half_euler = time_to_half_from_series(t_euler, P_euler_approx, K)

print()
print("Time when P(t) = K/2:")
print("  using exact formula:", t_half_exact)
print("  using forward Euler:", t_half_euler)
