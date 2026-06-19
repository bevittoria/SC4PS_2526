# HomeWork 07 - Random Numbers

This homework solves the five random-number exercises from `homework07.ipynb`.
All random samples are generated in C with the linear congruential generator
introduced in the notebook:

```text
x_{n+1} = 1664525 x_n + 1013904223 mod 2^32
```

Python is used only to plot the CSV files produced by the C program.

## Exercises

1. Simulate 100,000 fair coin tosses and plot the running fraction of heads.
2. Estimate pi with Monte Carlo sampling for several values of `N`.
3. Generate `Y = U^2` and compare the histogram with `f(y) = 1/(2 sqrt(y))`.
4. Generate an exponential variable with inverse transform sampling,
   `Y = -log(1 - U)/lambda`, using `lambda = 1.5`.
5. Plot the empirical CDF of the exponential sample and compare it with
   `F(y) = 1 - exp(-lambda y)`.

## Compile

```bash
gcc -std=c11 -Wall -Wextra -O2 random_exercises.c -o random_exercises -lm
```

## Run

```bash
./random_exercises
python3 plot_results.py
```

The C program writes:

- `coin_tosses.csv`
- `pi_errors.csv`
- `change_variables.csv`
- `exponential.csv`

The Python script writes the figures in `plots/`.

## Comments

In the coin-toss experiment, the running fraction of heads fluctuates strongly
at the beginning and then approaches `0.5`, as expected from the law of large
numbers.

The Monte Carlo estimate of pi improves as `N` grows, but the error is not
monotone because the sample is random. The expected order of convergence is
approximately `1/sqrt(N)`.

For the transformation `Y = U^2`, the density is large near zero:

```text
f_Y(y) = 1/(2 sqrt(y)), 0 < y <= 1.
```

For the exponential sample with `lambda = 1.5`, the inverse-transform formula
produces a histogram that follows:

```text
f(y) = lambda exp(-lambda y)
```

The empirical CDF follows the exact CDF:

```text
F(y) = 1 - exp(-lambda y).
```
