CFLAGS = -Wall -Wextra -Wconversion
# CFLAGS += -DNDEBUG

all: prefixsum-pthread prefixsum-seq

prefixsum-pthread: prefixsum-pthread.c secret.c
	gcc -std=c99 -O2 -pthread $(CFLAGS) $^ -o $@

prefixsum-seq: prefixsum-seq.c secret.c
	gcc -std=c99 -O2 -pthread $(CFLAGS) $^ -o $@

.PHONY: run
run: prefixsum-pthread prefixsum-seq
	@echo --- pthread ---
	@./prefixsum-pthread <<< "$(input)"
	@echo --- sequntial ---
	@./prefixsum-seq <<< "$(input)"
