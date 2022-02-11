# set CC=gcc rather than clang to support OpenMP in masOS
CFLAGS = -std=c99 -O2 -Wall -Wextra -Wconversion
LDFLAGS = -L/usr/local/opt/libomp/lib/
LDLIBS = -lomp
SHELL = bash

EXE = run

.PHONY: all
all: $(EXE)-omp $(EXE)-seq

$(EXE)-omp: CFLAGS += -fopenmp
$(EXE)-omp: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(EXE)-seq: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

.PHONY: run
run: $(EXE)-omp $(EXE)-seq
	@echo --- sequntial ---
	@/usr/bin/time diff --strip-trailing-cr <(./$(EXE)-seq < $(infile).in) $(infile).out
	@echo --- openmp ---
	@/usr/bin/time diff --strip-trailing-cr <(./$(EXE)-omp < $(infile).in) $(infile).out
