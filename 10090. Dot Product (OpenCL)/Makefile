CFLAGS = -std=c99 -O2 -Wall -Wextra -Wconversion
LDFLAGS =
LDLIBS = -lOpenCL
SHELL = bash

EXE = ./host


%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

.PHONY: run
run: $(EXE)
	diff <($(EXE) < $(infile).in) $(infile).out
