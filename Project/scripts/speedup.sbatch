#!/bin/bash
# Speedup test on Nova: serial version vs MPI version with 1..64 processes.
#
# Submit from the project folder on the LOGIN node:
#     mkdir -p results            (Slurm needs this folder for the log file)
#     sbatch scripts/speedup.sbatch
#
# Output:
#   results/speedup_<jobid>.log   everything the programs print
#   results/timings.csv           one line per run:
#       version,processes,run,time_s,work_min_s,work_avg_s,work_max_s
#
#SBATCH --job-name=climate_speedup
#SBATCH --nodes=1
#SBATCH --ntasks=64
#SBATCH --exclusive
#SBATCH --time=01:00:00
#SBATCH --partition=instruction
#SBATCH --account=f2026.cpre.4240.01
#SBATCH --output=results/speedup_%j.log

LIST=stations_all.txt     # all 2,502 candidate stations
DATA=data                 # folder with the .dly files
RUNS=3                    # repeat every test 3 times (we use the median later)
CSV=results/timings.csv

# Slurm starts the job in the folder where sbatch was run
source env.sh
mkdir -p results
echo "Job $SLURM_JOB_ID on $(hostname), $(date)"
echo "Stations in $LIST: $(wc -l < $LIST)"

echo "version,processes,run,time_s,work_min_s,work_avg_s,work_max_s" > $CSV

# ---- Warm-up run (NOT recorded) ----
# The first run reads the .dly files from shared storage "cold". Later runs
# may find them in the file cache, so we do one run first to make all the
# recorded runs fair.
echo "=== warm-up: np 8 (not recorded) ==="
mpirun --bind-to none -np 8 ./climate_mpi $LIST $DATA results/warmup

# ---- Serial version ----
for run in $(seq 1 $RUNS); do
    echo "=== serial, run $run ==="
    ./climate_serial $LIST $DATA results/serial > results/tmp_out.txt
    cat results/tmp_out.txt
    # "Time: 12.345 seconds"  -> 12.345
    t=$(awk '/^Time:/ {print $2}' results/tmp_out.txt)
    # serial has no "work per rank", so those columns stay empty
    echo "serial,1,$run,$t,,," >> $CSV
done

# ---- MPI version ----
for np in 1 2 4 8 16 32 64; do
    for run in $(seq 1 $RUNS); do
        echo "=== mpi, np $np, run $run ==="
        mpirun --bind-to none -np $np ./climate_mpi $LIST $DATA results/mpi_$np > results/tmp_out.txt
        cat results/tmp_out.txt
        t=$(awk '/^Time:/ {print $2}' results/tmp_out.txt)
        # "Work per rank: min A s, avg B s, max C s" -> fields 5, 8 and 11
        wmin=$(awk '/^Work per rank:/ {print $5}' results/tmp_out.txt)
        wavg=$(awk '/^Work per rank:/ {print $8}' results/tmp_out.txt)
        wmax=$(awk '/^Work per rank:/ {print $11}' results/tmp_out.txt)
        echo "mpi,$np,$run,$t,$wmin,$wavg,$wmax" >> $CSV
    done

    # Check: the MPI answer must be exactly the same as the serial answer
    if diff -q results/serial_stations.csv results/mpi_${np}_stations.csv > /dev/null; then
        echo "np $np: SAME as serial"
    else
        echo "np $np: WARNING - different from serial!"
    fi
done

rm -f results/tmp_out.txt
echo "Done at $(date)"
echo "--- $CSV ---"
cat $CSV
