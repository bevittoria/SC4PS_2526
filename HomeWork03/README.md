# HomeWork 03 - Matrix Multiplication

This program computes:

```text
C = A * B
```

where `A` is filled with the value `a`, `B` is filled with the value `b`, and the matrices are `N x N`.

Since every element of `A` is `a` and every element of `B` is `b`, each element of `C` is:

```text
C[i][j] = N * a * b
```

Here `1_N` is interpreted as an all-ones matrix, so the sum over `k` contains `N` equal terms.

## Compile

```bash
gcc matmul.c -o matmul
```

## Run

```bash
./matmul a b N fileout
```

Example:

```bash
./matmul 2 3 4 output.txt
```

The optional fifth argument is the number of repetitions for the benchmark:

```bash
./matmul 2 3 300 output.txt 3
```

## What the Code Does

- Allocates `A`, `B`, and `C` as contiguous 1D arrays.
- Tests all six loop orderings: `ijk`, `ikj`, `jik`, `jki`, `kij`, `kji`.
- Times each version using `clock()`.
- Checks a few elements quickly during the benchmark.
- Performs a full check before saving the output matrix.
- Saves matrix `C` to the file given by `fileout`.

## Benchmark Note

In C, arrays are stored in row-major order, so consecutive `j` indices are contiguous in memory. For this reason, loop orderings with `j` in the innermost loop, such as `ikj` and `kij`, are usually faster. They access memory more sequentially and use the cache better.
