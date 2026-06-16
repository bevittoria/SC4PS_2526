# HomeWork 05 - Stability of Legendre Polynomials

This homework compares numerical methods for computing ordinary Legendre polynomials:

```text
P_l(x), l = 0, ..., 50
```

for:

```text
x = 0.1, 0.5, 0.9, 0.99
```

Since `m = 0`, these are connected to spherical harmonics by:

```text
Y_l0(theta, phi) = sqrt((2*l + 1)/(4*pi)) P_l(cos(theta))
```

## Methods

The script implements:

- forward recurrence in standard double precision;
- high-precision reference recurrence using Python `decimal` with 110 decimal digits;
- Miller-style backward recurrence with `Ptilde[L+1] = 0`, `Ptilde[L] = 1`, then rescaling with `Ptilde[0]`.

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

## Run

```bash
python3 legendre_stability.py
```

The script writes:

- `legendre_errors.csv`
- `plots/forward_relative_error.png`
- `plots/backward_relative_error.png`
- `plots/forward_absolute_error.png`
- `plots/backward_absolute_error.png`

## Results

For the tested values of `x` and `lmax = 50`, the forward recurrence stays close to the high-precision reference. The errors are near double-precision roundoff, except when relative error is amplified by very small reference values.

The backward recurrence does not generally recover the ordinary Legendre polynomials. A three-term recurrence has two independent solutions, and the arbitrary final values used in the Miller-style experiment select an arbitrary combination of them. Rescaling with `Ptilde[0] = 1` fixes only the normalization, not the whole sequence.

Miller's algorithm is useful when the desired solution is minimal in the direction of backward propagation. For ordinary `P_l(x)` on `|x| < 1`, this experiment does not provide a clean dominant/minimal separation selecting `P_l`.

## Conclusion

For `x = 0.1, 0.5, 0.9, 0.99` and `lmax = 50`:

- the ordinary Legendre forward recurrence is stable;
- the arbitrary backward recurrence is not a reliable way to compute `P_l(x)`;
- errors in `P_l(cos(theta))` directly affect `Y_l0`, apart from the known normalization factor.
