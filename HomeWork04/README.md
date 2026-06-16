# HomeWork 04 - Fourier Transform

Answers to the guided worksheet in `LESSONS.md` from:

```text
https://github.com/alexninogh/myfft_tutorial
```

I checked the guided solution in `SOLUTIONS.md` and inspected the source files. I could not run `make run` locally because GNU GSL is not installed in this environment.

## Part 1 - Before Running

1. An FFT shows which frequencies are present in a sampled signal and how strong they are.
2. Sampling rate is the number of samples per second. The Nyquist frequency is half of the sampling rate, `f_N = f_s/2`, and is the highest frequency that can be represented without aliasing. Frequency resolution is the spacing between FFT bins, `Delta f = f_s/N = 1/T`.
3. The FFT routines used in this project assume a uniformly spaced time grid. If the samples are not equally spaced, the frequency bins do not correctly describe the data.

## Part 2 - Build and Run

Running `make run` should create:

- `output/good_sampling_signal.csv`
- `output/good_sampling_spectrum.csv`
- `output/undersampled_signal.csv`
- `output/undersampled_spectrum.csv`
- `output/short_record_signal.csv`
- `output/short_record_spectrum.csv`
- `output/coupled_oscillators_time.csv`
- `output/coupled_oscillators_spectrum.csv`

`src/sampling_demo.c` studies pure sampling issues.  
`src/coupled_oscillators_fft.c` studies a mechanics problem.  
In the well-sampled case, the two main frequencies are about `50 Hz` and `120 Hz`.

## Part 3 - Sampling and Aliasing

For the undersampled case:

```text
f_s = 128 Hz
f_N = f_s/2 = 64 Hz
```

The `120 Hz` component cannot be reconstructed correctly because it is above the Nyquist frequency. After sampling, it appears as a false low frequency:

```text
f_alias = |120 - 128| = 8 Hz
```

Physically, this happens because the signal is observed only at discrete times. Two different continuous signals can give the same sampled points, so a high frequency may look like a lower one.

## Part 4 - Frequency Resolution

For the short-record case:

```text
N = 64
f_s = 512 Hz
T = N/f_s = 64/512 = 0.125 s
Delta f = 1/T = 8 Hz
```

The signal contains `50 Hz` and `55 Hz`, which differ by only `5 Hz`. Since the FFT bin spacing is `8 Hz`, the two peaks are difficult to separate.

To improve this, I would increase the total acquisition time. Changing the plotting tool or file format would not improve the real frequency resolution.

## Part 5 - Coupled Oscillators

The equations of motion are:

```text
m x1'' = -(k + kc)x1 + kc x2
m x2'' =  kc x1 - (k + kc)x2
```

They describe two identical masses attached to walls and coupled to each other by a spring.

The two normal modes are:

- in-phase motion;
- out-of-phase motion.

The FFT of `x1(t)` shows more than one important frequency because the initial condition excites a combination of normal modes. Comparing the FFT peaks with the theoretical frequencies checks that the numerical simulation matches the expected physics.

The theoretical frequencies are:

```text
f_in  = sqrt(k/m) / (2*pi)
f_out = sqrt((k + 2*kc)/m) / (2*pi)
```

## Part 6 - Plot Inspection

The undersampled spectrum is the clearest plot for aliasing.  
The short-record spectrum is the clearest plot for limited frequency resolution.  
The coupled-oscillator FFT peaks should line up well with the theoretical normal-mode frequencies.

Small differences can come from finite observation time, FFT bin spacing, numerical integration tolerance, and the fact that the exact theoretical frequency may not fall exactly on an FFT bin.

## Part 7 - Small Code Modification

I choose Option B: change the observation time.

If the number of samples is increased while keeping the sampling step fixed, then:

```text
T = N * dt
Delta f = 1/T
```

So a larger `N` gives a longer total acquisition time and a smaller `Delta f`. I predict that the `50 Hz` and `55 Hz` peaks become easier to distinguish.

## Part 8 - Reflection

I learned that an FFT does not show the "true" continuous signal automatically: it shows the frequency content of the sampled data. Sampling rate controls aliasing, while total acquisition time controls frequency resolution.

I also learned that scientific software is easier to understand when source files, headers, output files, plots, and build commands are separated clearly. A useful improvement would be adding an extra example with noise or damping, to show how real experimental spectra become less clean.

## Errors and Issues

I checked the GitHub issues API for the repository and it returned an empty list, so I found no currently reported issues.

From local inspection, the main practical limitation is that the project requires GNU GSL. Without `gsl-config`, the `Makefile` stops with:

```text
GNU GSL was not found. Install GSL and make sure gsl-config is in PATH.
```

This is a dependency/setup issue, not an error in the tutorial code itself.
