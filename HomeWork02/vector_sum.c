#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 5) {
        printf("Usage: %s N a x y\n", argv[0]);
        printf("Example: %s 10 3 0.1 7.1\n", argv[0]);
        return 1;
    }

    long N = atol(argv[1]);
    double a = atof(argv[2]);
    double x_value = atof(argv[3]);
    double y_value = atof(argv[4]);

    if (N <= 0) {
        printf("Error: N must be positive.\n");
        return 1;
    }

    double *x = malloc(N * sizeof(double));
    double *y = malloc(N * sizeof(double));
    double *d = malloc(N * sizeof(double));

    if (x == NULL || y == NULL || d == NULL) {
        printf("Error: memory allocation failed. N may be too large.\n");
        free(x);
        free(y);
        free(d);
        return 1;
    }

    for (long i = 0; i < N; i++) {
        x[i] = x_value;
        y[i] = y_value;
    }

    for (long i = 0; i < N; i++) {
        d[i] = a * x[i] + y[i];
    }

    double expected = a * x_value + y_value;
    double tolerance = 1e-12;
    int correct = 1;

    for (long i = 0; i < N; i++) {
        if (fabs(d[i] - expected) > tolerance) {
            correct = 0;
            printf("Wrong value at i = %ld: d[i] = %.17g, expected = %.17g\n",
                   i, d[i], expected);
            break;
        }
    }

    printf("N = %ld\n", N);
    printf("a = %.17g, x = %.17g, y = %.17g\n", a, x_value, y_value);
    printf("Expected a*x + y = %.17g\n", expected);
    printf("Distance from 7.4 = %.17g\n", fabs(expected - 7.4));
    printf("Test: %s\n", correct ? "PASSED" : "FAILED");

    if (N <= 20) {
        for (long i = 0; i < N; i++) {
            printf("d[%ld] = %.17g\n", i, d[i]);
        }
    }

    free(x);
    free(y);
    free(d);

    return 0;
}
