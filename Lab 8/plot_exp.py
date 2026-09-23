import matplotlib.pyplot as plt

x = []
y = []
with open("exp.data", "r") as f:
    for line in f:
        a, b = line.split()
        x.append(float(a))
        y.append(float(b))

plt.plot(x, y, "o-")
plt.xlabel("x")
plt.ylabel("exp(x)")
plt.title("exp(x) on [0, 1]")
plt.grid(True)
plt.savefig("exp_plot.png", dpi=150)
plt.show()
