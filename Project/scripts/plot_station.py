# Plots the yearly average temperature for one station, with a trend line.
# Usage: python3 scripts/plot_station.py data/USC00130200.csv
import sys, csv
import matplotlib
matplotlib.use("Agg")                     # save to a file, no window needed
import matplotlib.pyplot as plt

path = sys.argv[1]

# 1. Group the daily highs and lows by year and month
months = {}                               # (year, month) -> [highs, lows]
for row in csv.DictReader(open(path)):
    year, month, day = row["date"].split("-")
    key = (int(year), int(month))
    months.setdefault(key, [[], []])
    if row["high_C"]:
        months[key][0].append(float(row["high_C"]))
    if row["low_C"]:
        months[key][1].append(float(row["low_C"]))

# 2. Yearly average, only for years where all 12 months have 20+ days
years, temps = [], []
for year in sorted({y for (y, m) in months}):
    monthly = []
    for m in range(1, 13):
        highs, lows = months.get((year, m), [[], []])
        if len(highs) < 20 or len(lows) < 20:
            break                         # incomplete month -> skip this year
        monthly.append((sum(highs) / len(highs) + sum(lows) / len(lows)) / 2)
    if len(monthly) == 12:
        years.append(year)
        temps.append(sum(monthly) / 12)

# 3. Straight-line fit (least squares) = the warming trend
n = len(years)
xbar = sum(years) / n
ybar = sum(temps) / n
slope = sum((x - xbar) * (y - ybar) for x, y in zip(years, temps)) / \
        sum((x - xbar) ** 2 for x in years)
trend_line = [ybar + slope * (x - xbar) for x in years]
print(f"{n} complete years, {years[0]}-{years[-1]}")
print(f"Warming trend: {slope * 10:.3f} C per decade")

# 4. Draw the graph
station = path.split("/")[-1].replace(".csv", "")
plt.figure(figsize=(10, 5))
plt.plot(years, temps, marker="o", markersize=3, label="Yearly average")
plt.plot(years, trend_line, "--", color="red",
         label=f"Trend: {slope * 10:+.2f} °C per decade")
plt.title(f"Average yearly temperature - station {station}")
plt.xlabel("Year")
plt.ylabel("Temperature (°C)")
plt.grid(alpha=0.3)
plt.legend()
plt.tight_layout()

out = path.replace(".csv", "_graph.png")
plt.savefig(out, dpi=150)
print("Saved", out)
