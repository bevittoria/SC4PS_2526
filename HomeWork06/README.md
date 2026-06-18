# HomeWork 06 - Split the Work

This homework starts from the DAXPY operation:

```text
d[i] = a*x[i] + y[i]
```

and computes it in two ways:

- one original loop over all vector elements;
- a chunked version with an outer loop over chunks.

The code also computes one partial sum for each chunk and checks that the sum of the partial sums is equal to the sum of the original result vector.

## Files

- `daxpy_chunks_hdf5.c`: C implementation.
- `daxpy_input.txt`: example input file.

## Input File

The input file uses the format:

```text
Variable = Value
```

Example:

```text
N = 100
chunk_size = 8
a = 3.0
x_value = 0.1
y_value = 7.1
output_file = daxpy_chunks.h5
```

The number of chunks is computed with integer ceiling:

```text
number_of_chunks = (N + chunk_size - 1) / chunk_size
```

## Compile

Compile with the HDF5 compiler wrapper:

```bash
h5cc -std=c11 -Wall -Wextra -O2 daxpy_chunks_hdf5.c -o daxpy_chunks_hdf5 -lm
```

or simply:

```bash
make
```

## Run

```bash
./daxpy_chunks_hdf5 daxpy_input.txt
```

or:

```bash
make run
```

## Output

The program prints:

- whether the chunked vector is equal to the original vector;
- the sum of the original result vector;
- the sum obtained from `partial_sum`;
- the absolute difference between the two sums;
- whether the two sums match within a floating-point tolerance.

The sums are checked with a tolerance because they are accumulated in a
different order. With double precision, values that are mathematically equal
can differ by tiny roundoff errors, for example around `1e-12`.

It also writes an HDF5 file containing:

```text
/x
/y
/d_original
/d_chunked
/partial_sum
/chunk_start
/chunk_end
/sum_original
/sum_chunks
/chunk_0000
/chunk_0001
...
```

If `h5dump` is installed, inspect the output with:

```bash
h5dump -n daxpy_chunks.h5
h5dump -d /partial_sum daxpy_chunks.h5
```
