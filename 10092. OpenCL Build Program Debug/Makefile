CFLAGS = -std=c99 -O2 -Wall -Wextra -Wconversion
LDFLAGS =
LDLIBS = -lOpenCL
SHELL = bash

EXE = ./main


$(EXE): main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

.PHONY: run
run: $(EXE)
	diff <($(EXE) < $(infile).in) $(infile).out
