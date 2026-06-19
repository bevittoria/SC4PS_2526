CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDLIBS = -lm

TARGET = fft_homework
SRC = src/fft_homework.c

.PHONY: all run plots clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

plots: run
	python3 scripts/plot_results.py

clean:
	rm -f $(TARGET)
	rm -rf output plots .matplotlib
