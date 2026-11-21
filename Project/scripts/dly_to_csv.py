# Converts a NOAA .dly file into a simple CSV: date, high_C, low_C
import sys, csv

path = sys.argv[1]
days = {}

for line in open(path):
    year  = int(line[11:15])
    month = int(line[15:17])
    what  = line[17:21]
    if what not in ("TMAX", "TMIN"):
        continue
    for d in range(31):
        start = 21 + d * 8
        value = int(line[start:start + 5])
        qflag = line[start + 6]
        if value == -9999 or qflag != " ":
            continue            # missing or failed quality check
        days.setdefault((year, month, d + 1), {})[what] = value / 10

out = path.replace(".dly", ".csv")
with open(out, "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["date", "high_C", "low_C"])
    for (y, m, d) in sorted(days):
        r = days[(y, m, d)]
        w.writerow([f"{y}-{m:02d}-{d:02d}", r.get("TMAX", ""), r.get("TMIN", "")])

print("Wrote", out)
