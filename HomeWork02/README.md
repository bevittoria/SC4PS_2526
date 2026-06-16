# HomeWork 02 - Vector Sum in C

The program computes the vector operation:

```text
d = a*x + y
```

where `a` is a scalar and `d`, `x`, `y` are vectors of dimension `N`. All elements of `x` have the same value, and all elements of `y` have the same value.

## Compile

```bash
gcc vector_sum.c -o vector_sum -lm
```

## Run

The command-line arguments are:

```bash
./vector_sum N a x y
```

Example required by the homework:

```bash
./vector_sum 10 3 0.1 7.1
./vector_sum 1000000 3 0.1 7.1
./vector_sum 100000000 3 0.1 7.1
```

The program checks that every element of `d` is equal to the expected value `a*x + y`, using a tolerance instead of exact equality.

## Notes

For `a = 3`, `x = 0.1`, `y = 7.1`, the mathematical result is:

```text
3*0.1 + 7.1 = 7.4
```

In C, decimal numbers such as `0.1` and `7.1` are not represented exactly in binary floating point. For this reason, direct comparison with `== 7.4` is not reliable, so the code uses a small tolerance.

In my test the printed value is close to `7.4`, but not exactly equal to it.

For `N = 10` and `N = 10^6`, the code should run without problems. For `N = 10^8`, the three `double` arrays require about:

```text
3 * 10^8 * 8 bytes = 2.4 GB
```

This may fail or become slow if the machine does not have enough available memory.
