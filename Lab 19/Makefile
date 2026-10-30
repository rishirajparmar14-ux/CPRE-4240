CC = gcc
FFLAGS = -O3 -Wall -std=c99 -fopenmp
LFLAGS = -lgomp -lm
OBJECTS = main.o \
matrix.o

ifeq ($(OS),Windows_NT)
SHELL = cmd.exe
RM = -del /Q /F
RUN = .\main.exe
else
RM = rm -f
RUN = ./main.exe
endif

.PHONY: clean help run

main.exe: $(OBJECTS)
	$(CC) $(OBJECTS) -o main.exe $(LFLAGS)

%.o: %.c matrix.h
	$(CC) $(FFLAGS) -c $<

run: main.exe
	$(RUN) 1 100000000
	$(RUN) 2 100000000
	$(RUN) 4 100000000
	$(RUN) 8 100000000
	$(RUN) 16 100000000

clean:
	$(RM) $(OBJECTS) main.exe

help:
	@echo "Valid targets:"
	@echo "  main.exe"
	@echo "  run:    runs main.exe with 1, 2, 4, 8 and 16 threads (N = 1e8)"
	@echo "  clean:  removes *.o and *.exe files"
