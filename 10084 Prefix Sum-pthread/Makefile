CFLAGS = -DNDEBUG

all: prefixsum-pthread prefixsum-seq

prefixsum-pthread: prefixsum-pthread.c secret.c
	gcc -std=c99 -O2 -pthread $(CFLAGS) $^ -o $@

prefixsum-seq: prefixsum-seq.c secret.c
	gcc -std=c99 -O2 -pthread $(CFLAGS) $^ -o $@
