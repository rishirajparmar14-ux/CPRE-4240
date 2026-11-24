# make_graphs.py - makes all the graphs for the report from the Nova results.
#
# Usage:   python3 scripts/make_graphs.py results_nova
# Reads:   timings.csv, compare.csv, serial_stations.csv, serial_regions.csv,
#          station_coords.txt   (all in the folder you give it)
# Writes:  <folder>/graphs/*.png  and prints a summary of the results
import sys, os, csv, statistics
import matplotlib
matplotlib.use("Agg")                       # save to files, no window
import matplotlib.pyplot as plt
from matplotlib.colors import TwoSlopeNorm

folder = sys.argv[1] if len(sys.argv) > 1 else "results_nova"
out = os.path.join(folder, "graphs")
os.makedirs(out, exist_ok=True)

def read_csv(name):
    with open(os.path.join(folder, name)) as f:
        return list(csv.DictReader(f))

def median_time(rows, **match):
    """median time_s of the rows whose columns match the given values"""
    times = [float(r["time_s"]) for r in rows
             if all(r[k] == str(v) for k, v in match.items())]
    return statistics.median(times)

def median_of(rows, column, **match):
    vals = [float(r[column]) for r in rows
            if all(r[k] == str(v) for k, v in match.items())]
    return statistics.median(vals)

def save(fig, name):
    fig.tight_layout()
    fig.savefig(os.path.join(out, name), dpi=150)
    plt.close(fig)
    print("  saved", os.path.join(out, name))

# ------------------------------------------------------------------
# Part 1: parallel performance
# ------------------------------------------------------------------
timings = read_csv("timings.csv")
compare = read_csv("compare.csv")

procs = [1, 2, 4, 8, 16, 32, 64]
t1 = median_time(timings, version="mpi", processes=1)      # baseline: 1 process
t_serial = median_time(timings, version="serial")
static_t = [median_time(timings, version="mpi", processes=p) for p in procs]
static_speedup = [t1 / t for t in static_t]

dyn_procs = [8, 16, 32, 64]
dyn_t = [median_time(compare, version="dynamic", processes=p, chunk=4) for p in dyn_procs]
dyn_speedup = [t1 / t for t in dyn_t]

# 1. speedup
fig, ax = plt.subplots(figsize=(7, 5))
ax.plot(procs, procs, "--", color="gray", label="Ideal")
ax.plot(procs, static_speedup, "o-", label="Static (MPI_Scatterv)")
ax.plot(dyn_procs, dyn_speedup, "s-", label="Dynamic (manager/worker)")
ax.set_xscale("log", base=2)
ax.set_xticks(procs)
ax.set_xticklabels(procs)
ax.set_xlabel("MPI processes")
ax.set_ylabel("Speedup (time on 1 process / time on N)")
ax.set_title("Speedup on Nova (2,502 stations, one 64-core node)")
ax.grid(alpha=0.3)
ax.legend()
for p, s in zip(dyn_procs[-1:], dyn_speedup[-1:]):
    ax.annotate(f"{s:.1f}x", (p, s), textcoords="offset points", xytext=(-30, 8))
save(fig, "1_speedup.png")

# 2. efficiency
fig, ax = plt.subplots(figsize=(7, 5))
ax.plot(procs, [100 * s / p for s, p in zip(static_speedup, procs)], "o-",
        label="Static")
ax.plot(dyn_procs, [100 * s / p for s, p in zip(dyn_speedup, dyn_procs)], "s-",
        label="Dynamic")
ax.set_xscale("log", base=2)
ax.set_xticks(procs)
ax.set_xticklabels(procs)
ax.set_ylim(0, 105)
ax.set_xlabel("MPI processes")
ax.set_ylabel("Efficiency (%)  = speedup / processes")
ax.set_title("Parallel efficiency")
ax.grid(alpha=0.3)
ax.legend()
save(fig, "2_efficiency.png")

# 3. load balance: slowest rank / average rank
static_bal = [median_of(compare, "work_max_s", version="static", processes=p) /
              median_of(compare, "work_avg_s", version="static", processes=p)
              for p in dyn_procs]
dyn_bal = [median_of(compare, "work_max_s", version="dynamic", processes=p, chunk=4) /
           median_of(compare, "work_avg_s", version="dynamic", processes=p, chunk=4)
           for p in dyn_procs]
fig, ax = plt.subplots(figsize=(7, 5))
x = range(len(dyn_procs))
ax.bar([i - 0.2 for i in x], static_bal, width=0.4, label="Static")
ax.bar([i + 0.2 for i in x], dyn_bal, width=0.4, label="Dynamic")
ax.axhline(1.0, color="gray", linestyle="--", label="Perfect balance")
ax.set_xticks(list(x))
ax.set_xticklabels(dyn_procs)
ax.set_ylim(0.9, max(static_bal) + 0.15)
ax.set_xlabel("MPI processes")
ax.set_ylabel("Slowest process work / average work")
ax.set_title("Load balance (closer to 1 is better)")
ax.legend()
save(fig, "3_load_balance.png")

# 4. chunk size (dynamic, 32 processes)
chunks = [1, 4, 16, 64]
chunk_t = [median_time(compare, version="dynamic", processes=32, chunk=c) for c in chunks]
static32 = median_time(compare, version="static", processes=32)
fig, ax = plt.subplots(figsize=(7, 5))
ax.bar([str(c) for c in chunks], chunk_t, color="tab:orange")
ax.axhline(static32, color="tab:blue", linestyle="--", label=f"Static ({static32:.3f} s)")
for i, t in enumerate(chunk_t):
    ax.text(i, t + 0.004, f"{t:.3f} s", ha="center")
ax.set_xlabel("Chunk size (stations handed out at a time)")
ax.set_ylabel("Time (s)")
ax.set_title("Dynamic version: effect of chunk size (32 processes)")
ax.legend()
save(fig, "4_chunk_size.png")

# ------------------------------------------------------------------
# Part 2: climate results
# ------------------------------------------------------------------
stations = read_csv("serial_stations.csv")
regions = read_csv("serial_regions.csv")
used = [r for r in stations if r["used"] == "yes"]

# station locations (fixed-width NOAA file: ID 1-11, lat 13-20, lon 22-30)
coords = {}
with open(os.path.join(folder, "station_coords.txt")) as f:
    for line in f:
        if len(line) > 30:
            coords[line[0:11]] = (float(line[12:20]), float(line[21:30]))

# 5. warming trend by state
regions_sorted = sorted(regions, key=lambda r: float(r["trend"]))
names = [r["region"].replace("US-", "") for r in regions_sorted]
trends = [float(r["trend"]) for r in regions_sorted]
fig, ax = plt.subplots(figsize=(7, 11))
colors = ["tab:red" if t > 0 else "tab:blue" for t in trends]
ax.barh(names, trends, color=colors)
ax.set_xlabel("Average warming trend (°C per decade, 1950-2025)")
ax.set_title("Warming trend by state")
ax.grid(axis="x", alpha=0.3)
ax.tick_params(axis="y", labelsize=8)
save(fig, "5_trend_by_state.png")

# 6. map of stations (contiguous US) colored by trend
lats, lons, vals = [], [], []
for r in used:
    if r["station"] in coords and not r["region"] in ("US-AK", "US-HI"):
        lat, lon = coords[r["station"]]
        lats.append(lat); lons.append(lon); vals.append(float(r["trend"]))
fig, ax = plt.subplots(figsize=(11, 6.5))
limit = max(abs(min(vals)), abs(max(vals)))
sc = ax.scatter(lons, lats, c=vals, cmap="RdBu_r", s=14,
                norm=TwoSlopeNorm(vmin=-limit, vcenter=0, vmax=limit))
fig.colorbar(sc, ax=ax, shrink=0.75, label="Warming trend (°C per decade)")
ax.set_xlabel("Longitude")
ax.set_ylabel("Latitude")
ax.set_title(f"Warming trend at {len(vals)} stations, 1950-2025 "
             "(red = warming, blue = cooling)")
ax.set_aspect(1.25)
ax.grid(alpha=0.2)
save(fig, "6_station_map.png")

# 7. change in hot days by state
regions_hot = sorted(regions, key=lambda r: float(r["hot_change"]))
names = [r["region"].replace("US-", "") for r in regions_hot]
changes = [float(r["hot_change"]) for r in regions_hot]
fig, ax = plt.subplots(figsize=(7, 11))
ax.barh(names, changes, color=["tab:red" if c > 0 else "tab:blue" for c in changes])
ax.set_xlabel("Change in days above 35 °C per year\n(1996-2025 minus 1951-1980)")
ax.set_title("Change in hot days by state")
ax.grid(axis="x", alpha=0.3)
ax.tick_params(axis="y", labelsize=8)
save(fig, "7_hot_days_by_state.png")

# ------------------------------------------------------------------
# Summary for the report
# ------------------------------------------------------------------
t = [float(r["trend"]) for r in used]
warming = [float(r["temp_now"]) - float(r["temp_before"]) for r in used]
print()
print("=== Summary ===")
print(f"Stations: {len(stations)} candidates, {len(used)} used")
print(f"Average trend: {statistics.mean(t):.3f} C/decade (median {statistics.median(t):.3f})")
print(f"Stations warming: {sum(x > 0 for x in t)} of {len(t)} "
      f"({100 * sum(x > 0 for x in t) / len(t):.0f}%)")
print(f"Average change 1951-1980 -> 1996-2025: {statistics.mean(warming):+.2f} C")
print(f"Serial time {t_serial:.3f} s, 1 process {t1:.3f} s")
print(f"Best static: {max(static_speedup):.1f}x on {procs[static_speedup.index(max(static_speedup))]} processes")
print(f"Best dynamic: {max(dyn_speedup):.1f}x on {dyn_procs[dyn_speedup.index(max(dyn_speedup))]} processes")
