CC = gcc
FFLAGS = -O3 -Wall -std=c99 -fopenmp
LFLAGS = -lgomp -lm
OBJECTS = main.o

ifeq ($(OS),Windows_NT)
SHELL = cmd.exe
RM = -del /Q /F
RUN = .\main.exe
else
RM = rm -f
RUN = ./main.exe
endif

.PHONY: clean help run plot

main.exe: $(OBJECTS)
	$(CC) $(OBJECTS) -o main.exe $(LFLAGS)

%.o: %.c
	$(CC) $(FFLAGS) -c $<

run: main.exe
	$(RUN) 1 1.0e-10
	$(RUN) 2 1.0e-10
	$(RUN) 4 1.0e-10
	$(RUN) 8 1.0e-10

plot: run
	python plot_bessel.py

clean:
	$(RM) $(OBJECTS) main.exe bessel.data

help:
	@echo "Valid targets:"
	@echo "  main.exe"
	@echo "  run:    runs main.exe with 1, 2, 4 and 8 threads (TOL = 1e-10)"
	@echo "  plot:   runs main.exe then plots J0(x) with plot_bessel.py"
	@echo "  clean:  removes *.o, *.exe and bessel.data files"
