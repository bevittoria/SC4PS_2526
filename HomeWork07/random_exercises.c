#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define COIN_TOSSES 100000
#define TRANSFORM_SAMPLES 200000
#define EXP_CDF_SAMPLES 50000


static uint32_t lcg32_next(uint32_t *state)
{
    const uint32_t a = 1664525u;
    const uint32_t c = 1013904223u;

    *state = a * (*state) + c;
    return *state;
}

static double lcg32_next_double(uint32_t *state)
{
    return lcg32_next(state) / 4294967296.0;
}

static FILE *open_output_file(const char *filename)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    return file;
}

static int compare_double(const void *pa, const void *pb)
{
    const double a = *(const double *)pa;
    const double b = *(const double *)pb;

    if (a < b) {
        return -1;
    }
    if (a > b) {
        return 1;
    }
    return 0;
}

static void write_coin_tosses(void)
{
    FILE *file = open_output_file("coin_tosses.csv");
    uint32_t state = 123456789u;
    int heads = 0;

    fprintf(file, "n,fraction_heads\n");

    for (int i = 1; i <= COIN_TOSSES; ++i) {
        const double u = lcg32_next_double(&state);
        const int toss = (u < 0.5) ? 1 : 0;

        heads += toss;

        if (i <= 100 || i % 1000 == 0) {
            fprintf(file, "%d,%.12f\n", i, (double)heads / i);
        }
    }

    fclose(file);
    printf("Coin tosses: final fraction of heads = %.8f\n",
           (double)heads / COIN_TOSSES);
}

static double estimate_pi(int n, uint32_t seed_x, uint32_t seed_y)
{
    uint32_t state_x = seed_x;
    uint32_t state_y = seed_y;
    int inside = 0;

    for (int i = 0; i < n; ++i) {
        const double x = lcg32_next_double(&state_x);
        const double y = lcg32_next_double(&state_y);

        if (x * x + y * y <= 1.0) {
            ++inside;
        }
    }

    return 4.0 * inside / n;
}

static void write_pi_estimates(void)
{
    const int sizes[] = {100, 1000, 10000, 100000, 1000000};
    const int n_sizes = (int)(sizeof(sizes) / sizeof(sizes[0]));
    const double pi_true = 3.14159265358979323846;
    FILE *file = open_output_file("pi_errors.csv");

    fprintf(file, "N,pi_estimate,error\n");

    for (int i = 0; i < n_sizes; ++i) {
        const double pi_estimate = estimate_pi(sizes[i], 123u, 987654321u);
        const double error = fabs(pi_estimate - pi_true);

        fprintf(file, "%d,%.12f,%.12f\n", sizes[i], pi_estimate, error);
        printf("Pi estimate at N = %d: %.8f, error = %.8f\n",
               sizes[i], pi_estimate, error);
    }

    fclose(file);
}

static void write_change_of_variables(void)
{
    FILE *file = open_output_file("change_variables.csv");
    uint32_t state = 13579u;

    fprintf(file, "y\n");

    for (int i = 0; i < TRANSFORM_SAMPLES; ++i) {
        const double u = lcg32_next_double(&state);
        const double y = u * u;

        fprintf(file, "%.12f\n", y);
    }

    fclose(file);
}

static void write_exponential_samples(void)
{
    FILE *file = open_output_file("exponential.csv");
    uint32_t state = 246813579u;
    const double lambda = 1.5;

    fprintf(file, "y\n");

    for (int i = 0; i < TRANSFORM_SAMPLES; ++i) {
        const double u = lcg32_next_double(&state);
        const double y = -log(1.0 - u) / lambda;

        fprintf(file, "%.12f\n", y);
    }

    fclose(file);
}

static void write_exponential_ecdf(void)
{
    FILE *file = open_output_file("exponential_ecdf.csv");
    double *sample = malloc((size_t)EXP_CDF_SAMPLES * sizeof(*sample));
    uint32_t state = 246813579u;
    const double lambda = 1.5;

    if (sample == NULL) {
        fprintf(stderr, "Could not allocate exponential sample.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < EXP_CDF_SAMPLES; ++i) {
        const double u = lcg32_next_double(&state);

        sample[i] = -log(1.0 - u) / lambda;
    }

    qsort(sample, (size_t)EXP_CDF_SAMPLES, sizeof(*sample), compare_double);
    fprintf(file, "y,empirical_cdf\n");

    for (int i = 0; i < EXP_CDF_SAMPLES; ++i) {
        fprintf(file, "%.12f,%.12f\n", sample[i],
                (double)(i + 1) / EXP_CDF_SAMPLES);
    }

    free(sample);
    fclose(file);
}

int main(void)
{
    write_coin_tosses();
    write_pi_estimates();
    write_change_of_variables();
    write_exponential_samples();
    write_exponential_ecdf();

    printf("CSV files written.\n");
    return 0;
}
