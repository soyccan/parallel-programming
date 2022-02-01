# set CC=gcc rather than clang in masOS
CFLAGS = -std=c99 -O2 -Wall -Wextra -Wconversion
LDFLAGS = -L/usr/local/opt/libomp/lib/
LDLIBS = -lomp

.PHONY: all
all: prefixsum-openmp prefixsum-seq

prefixsum-openmp: CFLAGS += -fopenmp
prefixsum-openmp: prefixsum-openmp.c secret.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

prefixsum-seq: prefixsum-seq.c secret.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

.PHONY: run
run: prefixsum-openmp prefixsum-seq
	@echo --- openmp ---
	@/usr/bin/time ./prefixsum-openmp <<< "$(input)"
	@echo --- sequntial ---
	@/usr/bin/time ./prefixsum-seq <<< "$(input)"
