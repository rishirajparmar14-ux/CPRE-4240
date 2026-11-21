# Build everything:   make
# Remove programs:    make clean
CC     = gcc
MPICC  = mpicc
CFLAGS = -Wall -O2

all: select_stations climate_serial climate_mpi

select_stations: src/select_stations.c
	$(CC) $(CFLAGS) src/select_stations.c -o select_stations

climate_serial: src/climate_serial.c src/ghcn.c src/ghcn.h
	$(CC) $(CFLAGS) src/climate_serial.c src/ghcn.c -o climate_serial -lm

climate_mpi: src/climate_mpi.c src/ghcn.c src/ghcn.h
	$(MPICC) $(CFLAGS) src/climate_mpi.c src/ghcn.c -o climate_mpi -lm

clean:
	rm -f select_stations climate_serial climate_mpi
