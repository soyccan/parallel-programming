CFLAGS = -std=c99 -pthread -Wall -Wextra -Wconversion
LDFLAGS = -pthread

all: job job_done

job_done: main.o sum.o job_done.o

job: main.o sum.o job.o

.PHONY: run
run: job job_done
	@echo --- pthread ---
	@./job_done <<< "$(input)"
	@echo --- sequntial ---
	@./job <<< "$(input)"
