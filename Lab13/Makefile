CC = gcc
FFLAGS = -O3 -Wall -std=c99
LFLAGS = -lm
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
	$(RUN)

clean:
	$(RM) $(OBJECTS) main.exe

help:
	@echo "Valid targets:"
	@echo "  main.exe"
	@echo "  run:    runs main.exe"
	@echo "  clean:  removes *.o and *.exe files"
