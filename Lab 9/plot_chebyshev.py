import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

N = 0
b = []
x = []
y = []

with open("chebyshev.data", "r") as f:
    for line in f:
        if line.startswith("# N ="):
            N = int(line.split("=")[1])
        elif line.startswith("# b ="):
            b = [float(v) for v in line.split("=")[1].split()]
        else:
            a, c = line.split()
            x.append(float(a))
            y.append(float(c))

coeffs = ", ".join("b%i = %g" % (i, v) for i, v in enumerate(b))

plt.plot(x, y, "-")
plt.xlabel("x")
plt.ylabel("p_%i(x)" % N)
plt.title("Chebyshev expansion, N = %i\n%s" % (N, coeffs))
plt.grid(True)
plt.tight_layout()
plt.savefig("chebyshev_plot.png", dpi=150)
print("Plot saved to chebyshev_plot.png")
