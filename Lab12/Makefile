# Lab 12: stack application - bisection method that finds every root
#         of f(x) on [a,b] by keeping unfinished intervals on a stack
CC = gcc
FFLAGS = -O3 -Wall
LFLAGS = -lm
OBJECTS = main.o \
Push.o \
Pop.o \
Peek.o \
DisplayStack.o \
GetStackSize.o \
DeleteStack.o \
Function.o \
Bisection.o

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

.PHONY: clean help run trace

main.exe: $(OBJECTS)
	$(CC) $(OBJECTS) -o main.exe $(LFLAGS)

%.o: %.c node.h
	$(CC) $(FFLAGS) -c $<

run: main.exe
	$(RUN) < input.txt

trace: main.exe
	$(RUN) < input_trace.txt

clean:
	$(RM) $(OBJECTS) main.exe

help:
	@echo "Valid targets:"
	@echo "  main.exe"
	@echo "  main.o"
	@echo "  Push.o"
	@echo "  Pop.o"
	@echo "  Peek.o"
	@echo "  DisplayStack.o"
	@echo "  GetStackSize.o"
	@echo "  DeleteStack.o"
	@echo "  Function.o"
	@echo "  Bisection.o"
	@echo "  run:    builds main.exe and runs it on input.txt"
	@echo "  trace:  builds main.exe and runs it on input_trace.txt (shows the stack)"
	@echo "  clean:  removes *.o and *.exe files"
