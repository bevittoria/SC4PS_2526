# HomeWork 05 - Stability of Legendre Polynomials

This homework studies the numerical stability of recurrence formulas for the
ordinary Legendre polynomials:

```text
P_l(x), l = 0, ..., 50
```

for:

```text
x = 0.1, 0.5, 0.9, 0.99
```

The numerical computation is done in C. Python is used only to plot the CSV
file produced by the C program.

## Files

- `legendre_stability.c`: C implementation of the recurrence tests.
- `plot_results.py`: Python plotting script.
- `Makefile`: build and run commands.

## Mathematical Background

The forward recurrence is Bonnet's recurrence:

```text
P_0(x) = 1
P_1(x) = x
P_{l+1}(x) = ((2l + 1)/(l + 1)) x P_l(x) - (l/(l + 1)) P_{l-1}(x)
```

The backward experiment uses:

```text
P_{l-1}(x) = ((2l + 1)/l) x P_l(x) - ((l + 1)/l) P_{l+1}(x)
```

with arbitrary starting values:

```text
Q_{L+1} = 0
Q_L = 1
```

Then the sequence is rescaled so that:

```text
Q_0 = 1
```

## Methods

The C program computes:

- a forward recurrence in `double`;
- a reference recurrence with double-double arithmetic implemented in C;
- a backward recurrence in `double`, starting from `L = 80`;
- absolute and relative errors with respect to the double-double reference.

The output is written to:

```text
legendre_errors.csv
```

## Compile

```bash
gcc -std=c11 -Wall -Wextra -O2 legendre_stability.c -o legendre_stability -lm
```

or simply:

```bash
make
```

## Run

```bash
./legendre_stability
```

or:

```bash
make run
```

To generate the plots:

```bash
python3 plot_results.py
```

or:

```bash
make plots
```

## Output

The C program writes:

- `legendre_errors.csv`

The plotting script writes:

- `plots/forward_relative_error.png`
- `plots/backward_relative_error.png`
- `plots/forward_absolute_error.png`
- `plots/backward_absolute_error.png`
- zoomed versions with `_zoom.png`

## Results

The forward recurrence stays close to the double-double reference for the tested
values of `x` and `lmax = 50`.

The backward recurrence is not reliable for computing the ordinary Legendre
polynomials in this experiment. A three-term recurrence has two independent
solutions, and arbitrary final values select an arbitrary combination of them.
Rescaling with `Q_0 = 1` fixes only the normalization, not the full sequence.

## Conclusion

For this homework:

- the forward recurrence is stable for the tested cases;
- the arbitrary backward recurrence is not a reliable method here;
- using a tolerance/error analysis is necessary when comparing floating-point
  results.
