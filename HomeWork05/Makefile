CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDLIBS = -lm

TARGET = legendre_stability

.PHONY: all run plots clean

all: $(TARGET)

$(TARGET): legendre_stability.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

plots: run
	python3 plot_results.py

clean:
	rm -f $(TARGET) legendre_errors.csv
	rm -rf plots .mplconfig
