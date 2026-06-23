# HomeWork 04 - Fourier Transform

This homework studies the Fourier transform with four examples:

1. a well-sampled signal;
2. an undersampled signal and aliasing;
3. a short time record and limited frequency resolution;
4. two coupled oscillators.

The C code generates the signals, computes the spectra with a radix-2 FFT, and
writes CSV files. The Python script reads the CSV files and creates the plots.

## Files

- `fft_homework.c`: C implementation of the FFT examples.
- `plot_results.py`: Python plotting script.
- `Makefile`: build and run commands.

CSV files are written in the main `HomeWork04` folder. Figures are written in
`plots/`.

## Compile

```bash
gcc -std=c11 -Wall -Wextra -O2 fft_homework.c -o fft_homework -lm
```

or simply:

```bash
make
```

## Run

```bash
./fft_homework
```

or:

```bash
make run
```

To generate plots:

```bash
python3 plot_results.py
```

or:

```bash
make plots
```

## Output

The program writes:

- `good_sampling_signal.csv`
- `good_sampling_spectrum.csv`
- `undersampled_signal.csv`
- `undersampled_spectrum.csv`
- `short_record_signal.csv`
- `short_record_spectrum.csv`
- `coupled_oscillators_time.csv`
- `coupled_oscillators_spectrum.csv`

The plotting script writes:

- `plots/good_sampling.png`
- `plots/undersampled.png`
- `plots/short_record.png`
- `plots/sampling_spectra_summary.png`
- `plots/coupled_oscillators.png`

## FFT

The FFT transforms a sampled signal from the time domain to the frequency
domain. It shows which frequencies are present and how strong they are.

The frequency spacing is:

```text
Delta f = f_s / N = 1 / T
```

where `f_s` is the sampling frequency, `N` is the number of samples, and `T` is
the total acquisition time.

The Nyquist frequency is:

```text
f_N = f_s / 2
```

Frequencies above `f_N` cannot be represented correctly and appear as aliased
lower frequencies.

## Good Sampling

The well-sampled signal contains:

```text
50 Hz
120 Hz
```

The program uses:

```text
f_s = 1024 Hz
N = 1024
Delta f = 1 Hz
```

Both frequencies are below the Nyquist frequency, so the spectrum shows clear
peaks at `50 Hz` and `120 Hz`.

## Aliasing

The undersampled signal contains the same two frequencies, but it is sampled at:

```text
f_s = 128 Hz
```

The Nyquist frequency is:

```text
f_N = 64 Hz
```

The `120 Hz` component is above the Nyquist limit, so it appears as:

```text
f_alias = |120 - 128| = 8 Hz
```

This happens because sampling observes the signal only at discrete times. A high
frequency can pass through the same sampled points as a lower frequency.

## Frequency Resolution

The short-record signal contains:

```text
50 Hz
55 Hz
```

The program uses:

```text
f_s = 512 Hz
N = 64
T = 64 / 512 = 0.125 s
Delta f = 1 / T = 8 Hz
```

The two frequencies are separated by only `5 Hz`, which is smaller than the FFT
bin spacing. For this reason, the two peaks are difficult to separate.

To improve the result, the total acquisition time should be increased. A longer
record gives a smaller `Delta f`.

## Coupled Oscillators

The coupled oscillator example models two identical masses connected to walls
and to each other by a spring.

The equations are:

```text
m x1'' = -(k + kc)x1 + kc x2
m x2'' =  kc x1 - (k + kc)x2
```

The two normal-mode frequencies are:

```text
f_in  = sqrt(k / m) / (2*pi)
f_out = sqrt((k + 2*kc) / m) / (2*pi)
```

With:

```text
m = 1
k = 25
kc = 7
```

the theoretical frequencies are approximately:

```text
f_in  = 0.79577 Hz
f_out = 0.99392 Hz
```

The FFT of `x1(t)` shows peaks close to these values. Small differences are due
to finite observation time and FFT bin spacing.

## Conclusion

Sampling rate controls aliasing. Total acquisition time controls frequency
resolution. The FFT is therefore not only a mathematical transform: its result
depends on how the signal was sampled.
