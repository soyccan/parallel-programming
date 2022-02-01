CFLAGS = -std=c99 -O2 -pthread -Wall -Wextra -Wconversion
LDFLAGS = -lpthread -lm

.PHONY: all
all: matrix_fast matrix_slow

matrix_slow.o: matrix_slow.c matrix.h
matrix_slow: matrix_slow.o main.o

matrix_fast.o: matrix_fast.c matrix.h
matrix_fast: matrix_fast.o main.o

ifdef input
.PHONY: run
run: matrix_fast matrix_slow
	@echo --- fast ---
	@/usr/bin/time ./matrix_fast <<< "$(input)"
	@echo --- slow ---
	@/usr/bin/time ./matrix_slow <<< "$(input)"

else ifdef infile
.PHONY: run
run: matrix_fast matrix_slow
	@echo --- fast ---
	@/usr/bin/time ./matrix_fast < "$(infile)"
	@echo --- slow ---
	@/usr/bin/time ./matrix_slow < "$(infile)"

endif

