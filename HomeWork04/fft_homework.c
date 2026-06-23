#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

static int is_power_of_two(long n)
{
    return n > 0 && (n & (n - 1)) == 0;
}

static long reverse_bits(long x, int bits)
{
    long reversed = 0;

    for (int i = 0; i < bits; ++i) {
        reversed = (reversed << 1) | (x & 1);
        x >>= 1;
    }

    return reversed;
}

/*
 * Iterative radix-2 Cooley-Tukey FFT.
 * The transform is computed in place and uses the convention:
 *
 *     X_k = sum_j x_j exp(-2 pi i j k / N)
 */
static void fft(complex double *data, long n)
{
    int bits = 0;

    while ((1L << bits) < n) {
        ++bits;
    }

    for (long i = 0; i < n; ++i) {
        long j = reverse_bits(i, bits);

        if (j > i) {
            complex double tmp = data[i];
            data[i] = data[j];
            data[j] = tmp;
        }
    }

    for (long length = 2; length <= n; length *= 2) {
        const double angle = -2.0 * PI / length;
        const complex double w_length = cos(angle) + I * sin(angle);

        for (long start = 0; start < n; start += length) {
            complex double w = 1.0 + 0.0 * I;

            for (long j = 0; j < length / 2; ++j) {
                complex double even = data[start + j];
                complex double odd = w * data[start + j + length / 2];

                data[start + j] = even + odd;
                data[start + j + length / 2] = even - odd;
                w *= w_length;
            }
        }
    }
}

static FILE *open_file(const char *filename)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    return file;
}

static double two_sine_signal(double t,
                              double f1,
                              double a1,
                              double f2,
                              double a2)
{
    return a1 * sin(2.0 * PI * f1 * t) + a2 * sin(2.0 * PI * f2 * t);
}

static void write_signal_csv(const char *filename,
                             const double *signal,
                             long n,
                             double sample_rate)
{
    FILE *file = open_file(filename);

    fprintf(file, "time,value\n");

    for (long i = 0; i < n; ++i) {
        fprintf(file, "%.12f,%.12f\n", i / sample_rate, signal[i]);
    }

    fclose(file);
}

static void write_spectrum_csv(const char *filename,
                               const double *signal,
                               long n,
                               double sample_rate)
{
    FILE *file = open_file(filename);
    complex double *data = malloc((size_t)n * sizeof(*data));

    if (data == NULL) {
        fprintf(stderr, "Allocation failed for FFT data.\n");
        exit(EXIT_FAILURE);
    }

    for (long i = 0; i < n; ++i) {
        data[i] = signal[i] + 0.0 * I;
    }

    fft(data, n);

    fprintf(file, "frequency,amplitude\n");

    for (long k = 0; k <= n / 2; ++k) {
        double frequency = k * sample_rate / n;
        double amplitude = cabs(data[k]) / n;

        if (k != 0 && k != n / 2) {
            amplitude *= 2.0;
        }

        fprintf(file, "%.12f,%.12f\n", frequency, amplitude);
    }

    free(data);
    fclose(file);
}

static void report_two_largest_peaks(const char *label,
                                     const double *signal,
                                     long n,
                                     double sample_rate)
{
    complex double *data = malloc((size_t)n * sizeof(*data));
    double best_amp[2] = {0.0, 0.0};
    double best_freq[2] = {0.0, 0.0};

    if (data == NULL) {
        fprintf(stderr, "Allocation failed for peak search.\n");
        exit(EXIT_FAILURE);
    }

    for (long i = 0; i < n; ++i) {
        data[i] = signal[i] + 0.0 * I;
    }

    fft(data, n);

    for (long k = 1; k <= n / 2; ++k) {
        double amplitude = cabs(data[k]) / n;
        double frequency = k * sample_rate / n;

        if (k != n / 2) {
            amplitude *= 2.0;
        }

        if (amplitude > best_amp[0]) {
            best_amp[1] = best_amp[0];
            best_freq[1] = best_freq[0];
            best_amp[0] = amplitude;
            best_freq[0] = frequency;
        } else if (amplitude > best_amp[1]) {
            best_amp[1] = amplitude;
            best_freq[1] = frequency;
        }
    }

    printf("%s peak 1: f = %.5f Hz, amplitude = %.5f\n",
           label, best_freq[0], best_amp[0]);
    printf("%s peak 2: f = %.5f Hz, amplitude = %.5f\n",
           label, best_freq[1], best_amp[1]);

    free(data);
}

static void run_sampling_case(const char *label,
                              const char *signal_file,
                              const char *spectrum_file,
                              long n,
                              double sample_rate,
                              double f1,
                              double a1,
                              double f2,
                              double a2)
{
    double *signal = malloc((size_t)n * sizeof(*signal));

    if (!is_power_of_two(n)) {
        fprintf(stderr, "N must be a power of two for this FFT.\n");
        exit(EXIT_FAILURE);
    }

    if (signal == NULL) {
        fprintf(stderr, "Allocation failed for signal.\n");
        exit(EXIT_FAILURE);
    }

    for (long i = 0; i < n; ++i) {
        double t = i / sample_rate;
        signal[i] = two_sine_signal(t, f1, a1, f2, a2);
    }

    write_signal_csv(signal_file, signal, n, sample_rate);
    write_spectrum_csv(spectrum_file, signal, n, sample_rate);
    report_two_largest_peaks(label, signal, n, sample_rate);

    free(signal);
}

static void run_coupled_oscillators(void)
{
    const long n = 8192;
    const double sample_rate = 64.0;
    const double m = 1.0;
    const double k = 25.0;
    const double kc = 7.0;
    const double omega_in = sqrt(k / m);
    const double omega_out = sqrt((k + 2.0 * kc) / m);
    const double f_in = omega_in / (2.0 * PI);
    const double f_out = omega_out / (2.0 * PI);
    FILE *time_file = open_file("coupled_oscillators_time.csv");
    double *x1 = malloc((size_t)n * sizeof(*x1));

    if (x1 == NULL) {
        fprintf(stderr, "Allocation failed for oscillator data.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(time_file, "time,x1,x2\n");

    for (long i = 0; i < n; ++i) {
        double t = i / sample_rate;
        double in_phase = cos(omega_in * t);
        double out_phase = cos(omega_out * t);
        double x2;

        x1[i] = 0.5 * (in_phase + out_phase);
        x2 = 0.5 * (in_phase - out_phase);
        fprintf(time_file, "%.12f,%.12f,%.12f\n", t, x1[i], x2);
    }

    fclose(time_file);

    write_spectrum_csv("coupled_oscillators_spectrum.csv", x1, n, sample_rate);
    report_two_largest_peaks("coupled oscillators", x1, n, sample_rate);

    printf("theoretical in-phase frequency  = %.5f Hz\n", f_in);
    printf("theoretical out-phase frequency = %.5f Hz\n", f_out);

    free(x1);
}

int main(void)
{
    run_sampling_case("good sampling",
                      "good_sampling_signal.csv",
                      "good_sampling_spectrum.csv",
                      1024, 1024.0,
                      50.0, 1.0,
                      120.0, 0.7);

    run_sampling_case("undersampled",
                      "undersampled_signal.csv",
                      "undersampled_spectrum.csv",
                      256, 128.0,
                      50.0, 1.0,
                      120.0, 0.7);

    run_sampling_case("short record",
                      "short_record_signal.csv",
                      "short_record_spectrum.csv",
                      64, 512.0,
                      50.0, 1.0,
                      55.0, 1.0);

    run_coupled_oscillators();

    printf("CSV files written.\n");
    return 0;
}
