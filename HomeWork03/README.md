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

## What the Code Does

- Allocates `A`, `B`, and `C` as contiguous 1D arrays.
- Tests two loop orderings: `ijk` and `ikj`.
- Times each version using `clock()`.
- Checks every element of the result matrix.
- Saves matrix `C` to the file given by `fileout`.

## Benchmark Note

In C, arrays are stored in row-major order, so consecutive `j` indices are contiguous in memory. For this reason, `ikj` is usually faster than `ijk`: the innermost loop moves along contiguous memory.
