CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDLIBS = -lm

.PHONY: all run plots clean

all: random_exercises

random_exercises: random_exercises.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

run: random_exercises
	./random_exercises

plots: run
	python3 plot_results.py

clean:
	rm -f random_exercises *.csv
	rm -rf plots .matplotlib
