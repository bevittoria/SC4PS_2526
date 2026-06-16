#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static double absolute_value(double value)
{
    return value < 0.0 ? -value : value;
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "Usage: %s N a x y\n", argv[0]);
        fprintf(stderr, "Example: %s 10 3 0.1 7.1\n", argv[0]);
        return 1;
    }

    errno = 0;
    char *end = NULL;
    unsigned long N = strtoul(argv[1], &end, 10);
    if (errno != 0 || end == argv[1] || *end != '\0' || N == 0) {
        fprintf(stderr, "Error: N must be a positive integer.\n");
        return 1;
    }

    errno = 0;
    double a = strtod(argv[2], &end);
    if (errno != 0 || end == argv[2] || *end != '\0') {
        fprintf(stderr, "Error: a must be a number.\n");
        return 1;
    }

    errno = 0;
    double x_value = strtod(argv[3], &end);
    if (errno != 0 || end == argv[3] || *end != '\0') {
        fprintf(stderr, "Error: x must be a number.\n");
        return 1;
    }

    errno = 0;
    double y_value = strtod(argv[4], &end);
    if (errno != 0 || end == argv[4] || *end != '\0') {
        fprintf(stderr, "Error: y must be a number.\n");
        return 1;
    }

    if (N > (unsigned long)-1 / sizeof(double)) {
        fprintf(stderr, "Error: N is too large.\n");
        return 1;
    }

    double *x = malloc(N * sizeof(*x));
    double *y = malloc(N * sizeof(*y));
    double *d = malloc(N * sizeof(*d));

    if (x == NULL || y == NULL || d == NULL) {
        fprintf(stderr, "Error: not enough memory for N = %lu.\n", N);
        free(x);
        free(y);
        free(d);
        return 1;
    }

    for (unsigned long i = 0; i < N; i++) {
        x[i] = x_value;
        y[i] = y_value;
        d[i] = a * x[i] + y[i];
    }

    double expected = a * x_value + y_value;
    double target = 7.4;
    double tolerance = 1e-12;
    int ok = 1;

    for (unsigned long i = 0; i < N; i++) {
        double error = absolute_value(d[i] - expected);
        if (error > tolerance) {
            printf("First wrong element: d[%lu] = %.17g, expected %.17g\n",
                   i, d[i], expected);
            ok = 0;
            break;
        }
    }

    printf("N = %lu\n", N);
    printf("a = %.17g, x = %.17g, y = %.17g\n", a, x_value, y_value);
    printf("Expected value: %.17g\n", expected);
    printf("Distance from 7.4: %.17g\n", absolute_value(expected - target));
    printf("First element d[0]: %.17g\n", d[0]);
    printf("Test: %s\n", ok ? "PASSED" : "FAILED");

    free(x);
    free(y);
    free(d);

    return ok ? 0 : 1;
}
