CC := h5cc
CFLAGS := -std=c11 -Wall -Wextra -O2
LDFLAGS := -lm

TARGET := daxpy_chunks_hdf5
INPUT := daxpy_input.txt

.PHONY: all run clean

all: $(TARGET)

$(TARGET): daxpy_chunks_hdf5.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

run: $(TARGET)
	./$(TARGET) $(INPUT)

clean:
	rm -f $(TARGET) daxpy_chunks.h5
