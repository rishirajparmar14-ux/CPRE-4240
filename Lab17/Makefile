CC = gcc
FFLAGS = -O3 -Wall -std=c99 -fopenmp
LFLAGS = -lgomp -lm
OBJECTS = blur.o \
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
	$(RUN) 1 5000
	$(RUN) 2 5000
	$(RUN) 4 5000
	$(RUN) 8 5000

clean:
	$(RM) $(OBJECTS) main.exe

help:
	@echo "Valid targets:"
	@echo "  main.exe"
	@echo "  run:    runs main.exe with 1, 2, 4 and 8 threads"
	@echo "  clean:  removes *.o and *.exe files"
