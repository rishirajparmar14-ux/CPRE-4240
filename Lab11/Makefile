# Lab 11: linked list application - polynomial representation,
#         addition and evaluation
CC = gcc
FFLAGS = -O3 -Wall
LFLAGS =
OBJECTS = main.o \
CreateTerm.o \
InsertTerm.o \
ReadPoly.o \
PrintPoly.o \
AddPoly.o \
EvaluatePoly.o \
DeletePoly.o

# on Windows use cmd.exe commands, so make works the same whether it is
# started from PowerShell, cmd, or Git Bash
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
	$(CC) $(LFLAGS) $(OBJECTS) -o main.exe

%.o: %.c poly.h
	$(CC) $(FFLAGS) -c $<

run: main.exe
	$(RUN) < input.txt

clean:
	$(RM) $(OBJECTS) main.exe

help:
	@echo "Valid targets:"
	@echo "  main.exe"
	@echo "  main.o"
	@echo "  CreateTerm.o"
	@echo "  InsertTerm.o"
	@echo "  ReadPoly.o"
	@echo "  PrintPoly.o"
	@echo "  AddPoly.o"
	@echo "  EvaluatePoly.o"
	@echo "  DeletePoly.o"
	@echo "  run:    builds main.exe and runs it on input.txt"
	@echo "  clean:  removes *.o and *.exe files"
