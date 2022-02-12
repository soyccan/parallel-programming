CFLAGS = -std=c99 -O2 -Wall -Wextra -Wconversion
LDFLAGS =
LDLIBS = -lOpenCL
SHELL = bash

MAIN_PROG = ./vecdot_cl

.PHONY: all
all: vecdot_seq vecdot_cl

vecdot_seq: src.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

vecdot_cl: host.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

.PHONY: run
run: $(MAIN_PROG)
	diff <($(MAIN_PROG) < $(infile).in) $(infile).out

.PHONY: run_seq
run_seq: $(MAIN_PROG) vecdot_seq
	diff <($(MAIN_PROG) <<< "$(input)") <(./vecdot_seq <<< "$(input)")
