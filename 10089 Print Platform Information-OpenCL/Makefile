CFLAGS = -std=c99 -O2 -Wall -Wextra -Wconversion
LDFLAGS =
LDLIBS = -lOpenCL
SHELL = bash

EXE = run

.PHONY: all
all: $(EXE)

$(EXE): main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@
