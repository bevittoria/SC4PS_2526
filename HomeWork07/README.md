# HomeWork 07 - Random Numbers

This homework studies simple random-number simulations:

1. coin tosses and the law of large numbers;
2. Monte Carlo estimate of pi;
3. change of variables `Y = U^2`;
4. inverse transform sampling for an exponential distribution;
5. empirical CDF of the exponential sample.

## Compile

```bash
gcc random_exercises.c -o random_exercises -lm
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

The Python script writes plots in `plots/`.

## Comments

For the coin toss experiment, the running fraction of heads fluctuates at the beginning but approaches `0.5` as the number of tosses grows. This is the law of large numbers.

For the Monte Carlo estimate of pi, the error decreases when `N` increases, but not smoothly because the method is random. The expected scaling is approximately proportional to `1/sqrt(N)`.

For `Y = U^2`, the analytic density is:

```text
f_Y(y) = 1/(2 sqrt(y))
```

For the exponential variable with `lambda = 1.5`, the inverse transform formula is:

```text
Y = -log(1 - U)/lambda
```

The empirical CDF follows the exact CDF:

```text
F(y) = 1 - exp(-lambda y)
```
