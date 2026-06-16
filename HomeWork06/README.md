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

On AlmaLinux/CloudVeneto, first install HDF5 development files if needed:

```bash
sudo dnf install hdf5-devel
```

Then compile with the HDF5 compiler wrapper:

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
- whether the two sums match.

It also writes an HDF5 file containing:

```text
/config
/vectors/x
/vectors/y
/vectors/d_reference
/vectors/d_chunked
/chunks/start_index
/chunks/end_index
/chunks/length
/chunks/partial_sum
/chunks/chunk_0000/x
/chunks/chunk_0000/y
/chunks/chunk_0000/d
/chunks/chunk_0000/partial_sum
...
```

If `h5dump` is installed, inspect the output with:

```bash
h5dump -n daxpy_chunks.h5
h5dump -d /chunks/partial_sum daxpy_chunks.h5
```

## Note

I could not compile this program locally because `h5cc` is not installed in this environment. It is intended to compile on the CloudVeneto Linux machine after installing `hdf5-devel`.
