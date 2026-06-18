#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t lcg_next(uint32_t *state)
{
    *state = 1664525u * (*state) + 1013904223u;
    return *state;
}

double uniform(uint32_t *state)
{
    return lcg_next(state) / 4294967296.0;
}

void coin_tosses(int N)
{
    FILE *file = fopen("coin_tosses.csv", "w");
    uint32_t state = 12345u;
    int heads = 0;

    fprintf(file, "n,fraction_heads\n");

    for (int i = 1; i <= N; i++) {
        double u = uniform(&state);

        if (u < 0.5) {
            heads++;
        }

        if (i <= 100 || i % 1000 == 0) {
            fprintf(file, "%d,%.12f\n", i, (double)heads / i);
        }
    }

    fclose(file);

    printf("Coin tosses: final fraction of heads = %.6f\n", (double)heads / N);
}

void estimate_pi(void)
{
    FILE *file = fopen("pi_errors.csv", "w");
    uint32_t state_x = 11111u;
    uint32_t state_y = 99999u;
    int inside = 0;
    int sample_sizes[] = {100, 1000, 10000, 100000, 1000000};
    int next_sample = 0;
    int max_N = sample_sizes[4];
    double true_pi = 3.141592653589793;

    fprintf(file, "N,pi_estimate,error\n");

    for (int i = 1; i <= max_N; i++) {
        double x = uniform(&state_x);
        double y = uniform(&state_y);

        if (x * x + y * y <= 1.0) {
            inside++;
        }

        if (i == sample_sizes[next_sample]) {
            double pi_estimate = 4.0 * inside / i;
            double error = fabs(pi_estimate - true_pi);

            fprintf(file, "%d,%.12f,%.12f\n", i, pi_estimate, error);
            printf("Pi estimate at N = %d: %.8f, error = %.8f\n",
                   i, pi_estimate, error);

            next_sample++;
            if (next_sample == 5) {
                break;
            }
        }
    }

    fclose(file);
}

void change_of_variables(int N)
{
    FILE *file = fopen("change_variables.csv", "w");
    uint32_t state = 22222u;

    fprintf(file, "y\n");

    for (int i = 0; i < N; i++) {
        double u = uniform(&state);
        double y = u * u;
        fprintf(file, "%.12f\n", y);
    }

    fclose(file);
}

void inverse_exponential(int N)
{
    FILE *file = fopen("exponential.csv", "w");
    uint32_t state = 33333u;
    double lambda = 1.5;

    fprintf(file, "y\n");

    for (int i = 0; i < N; i++) {
        double u = uniform(&state);
        double y = -log(1.0 - u) / lambda;
        fprintf(file, "%.12f\n", y);
    }

    fclose(file);
}

int main(void)
{
    int N = 100000;

    coin_tosses(N);
    estimate_pi();
    change_of_variables(N);
    inverse_exponential(N);

    printf("CSV files written.\n");

    return 0;
}
