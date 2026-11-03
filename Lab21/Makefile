CC = mpicc
FFLAGS = -O3 -Wall -std=c99
LFLAGS =
OBJECTS = main.o

ifeq ($(OS),Windows_NT)
SHELL = cmd.exe
RM = -del /Q /F
MPIRUN = mpiexec -n
EXE = main.exe
else
RM = rm -f
MPIRUN = mpirun -np
EXE = ./main.exe
endif

.PHONY: clean help run

main.exe: $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o main.exe

%.o: %.c
	$(CC) $(FFLAGS) -c $<

run: main.exe
	$(MPIRUN) 2 $(EXE)
	$(MPIRUN) 4 $(EXE)
	$(MPIRUN) 8 $(EXE)

clean:
	$(RM) $(OBJECTS) main.exe *.data

help:
	@echo "Valid targets:"
	@echo "  main.exe"
	@echo "  run:    passes the baton around a ring of 2, 4 and 8 MPI processes"
	@echo "  clean:  removes *.o, *.exe and *.data files"
