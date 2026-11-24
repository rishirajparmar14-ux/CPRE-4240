#!/bin/bash
# Compares the STATIC version (climate_mpi) with the DYNAMIC version
# (climate_mpi_dynamic) on 8, 16, 32 and 64 processes, 3 runs each.
# Also tries a few chunk sizes for the dynamic version at 32 processes.
# Results: results/compare.csv
#
# Submit with:   sbatch scripts/compare.sbatch
#
#SBATCH --job-name=climate_compare
#SBATCH --partition=instruction
#SBATCH --account=f2026.cpre.4240.01
#SBATCH --nodes=1
#SBATCH --ntasks=64
#SBATCH --exclusive
#SBATCH --time=00:30:00
#SBATCH --output=results/compare_%j.log

source env.sh
mkdir -p results
STATIONS=stations_all.txt
OUT=results/compare.csv
echo "Node: $(hostname), started $(date)"

# warm-up (not recorded) so the files are already in memory
mpirun --bind-to none -np 8 ./climate_mpi $STATIONS data results/warmup > /dev/null
./climate_serial $STATIONS data results/serial > /dev/null

# run one test and add a line to the CSV
# arguments: version processes chunk run
run_test() {
    local version=$1 p=$2 chunk=$3 run=$4
    if [ "$version" = "static" ]; then
        mpirun --bind-to none -np $p ./climate_mpi $STATIONS data results/cmp > results/last_run.txt
    else
        mpirun --bind-to none -np $p ./climate_mpi_dynamic $STATIONS data results/cmp $chunk > results/last_run.txt
    fi
    t=$(awk '/^Time:/ {print $2}' results/last_run.txt)
    w=$(awk '/^Work per rank:/ {print $5","$8","$11}' results/last_run.txt)
    echo "$version,$p,$chunk,$run,$t,$w" >> $OUT
    echo "$version np=$p chunk=$chunk run=$run: $t s  (work min/avg/max: $w)"
    if diff -q results/serial_stations.csv results/cmp_stations.csv > /dev/null; then
        echo "   SAME as serial"
    else
        echo "   WARNING: DIFFERENT from serial"
    fi
}

echo "version,processes,chunk,run,time_s,work_min_s,work_avg_s,work_max_s" > $OUT

# static vs dynamic (chunk 4)
for p in 8 16 32 64; do
    for run in 1 2 3; do
        run_test static  $p - $run
        run_test dynamic $p 4 $run
    done
done

# effect of the chunk size (dynamic, 32 processes)
for chunk in 1 16 64; do
    for run in 1 2 3; do
        run_test dynamic 32 $chunk $run
    done
done

echo "Finished $(date)"
