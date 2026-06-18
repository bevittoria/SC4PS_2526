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

## Install HDF5 on macOS

This homework can be compiled locally on macOS. HDF5 is not installed by
default, so first install Homebrew if the `brew` command is missing:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

After the installation, follow the commands printed by Homebrew to add it to
the shell path. On Apple Silicon Macs, they are usually:

```bash
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"
```

Then install HDF5:

```bash
brew install hdf5
```

## Compile on macOS

Compile with the HDF5 compiler wrapper:

```bash
h5cc -std=c11 -Wall -Wextra -O2 daxpy_chunks_hdf5.c -o daxpy_chunks_hdf5 -lm
```

If `h5cc` is still not found, use the full Homebrew path:

```bash
/opt/homebrew/bin/h5cc -std=c11 -Wall -Wextra -O2 daxpy_chunks_hdf5.c -o daxpy_chunks_hdf5 -lm
```

or simply run:

```bash
make
```

## Compile on AlmaLinux/CloudVeneto

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

## Note

The command `sudo dnf install hdf5-devel` is only for Linux distributions that
use `dnf`, such as AlmaLinux. It does not work on macOS. On macOS, use
Homebrew and `brew install hdf5`.
