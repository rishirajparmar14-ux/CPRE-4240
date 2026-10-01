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
	@echo "  run:    runs main.exe with input.txt"
	@echo "  trace:  runs main.exe with input_trace.txt (shows steps)"
	@echo "  clean:  removes *.o and *.exe files"
