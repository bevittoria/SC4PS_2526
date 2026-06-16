#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define IDX(i, j, n) ((i) * (n) + (j))

typedef void (*matmul_fn)(const double *, const double *, double *, size_t);

typedef struct {
    const char *name;
    matmul_fn function;
    double time;
} Variant;

static double absolute_value(double value)
{
    return value < 0.0 ? -value : value;
}

static double seconds(void)
{
    return (double)clock() / CLOCKS_PER_SEC;
}

static void zero_matrix(double *matrix, size_t N)
{
    memset(matrix, 0, N * N * sizeof(*matrix));
}

static void fill_matrix(double *matrix, size_t N, double value)
{
    for (size_t i = 0; i < N * N; i++) {
        matrix[i] = value;
    }
}

static int almost_equal(double x, double y)
{
    return absolute_value(x - y) < 1e-9;
}

static int quick_check(const double *C, size_t N, double expected)
{
    return almost_equal(C[IDX(0, 0, N)], expected) &&
           almost_equal(C[IDX(0, N - 1, N)], expected) &&
           almost_equal(C[IDX(N - 1, 0, N)], expected) &&
           almost_equal(C[IDX(N - 1, N - 1, N)], expected) &&
           almost_equal(C[IDX(N / 2, N / 2, N)], expected);
}

static int full_check(const double *C, size_t N, double expected)
{
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            if (!almost_equal(C[IDX(i, j, N)], expected)) {
                printf("Wrong value at C[%zu][%zu] = %.17g, expected %.17g\n",
                       i, j, C[IDX(i, j, N)], expected);
                return 0;
            }
        }
    }

    return 1;
}

static int save_matrix(const char *filename, const double *C, size_t N)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }

    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            fprintf(file, "%.10g", C[IDX(i, j, N)]);
            if (j + 1 < N) {
                fputc(' ', file);
            }
        }
        fputc('\n', file);
    }

    fclose(file);
    return 1;
}

static void matmul_ijk(const double *A, const double *B, double *C, size_t N)
{
    zero_matrix(C, N);
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++)
            for (size_t k = 0; k < N; k++)
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
}

static void matmul_ikj(const double *A, const double *B, double *C, size_t N)
{
    zero_matrix(C, N);
    for (size_t i = 0; i < N; i++)
        for (size_t k = 0; k < N; k++) {
            double aik = A[IDX(i, k, N)];
            for (size_t j = 0; j < N; j++)
                C[IDX(i, j, N)] += aik * B[IDX(k, j, N)];
        }
}

static void matmul_jik(const double *A, const double *B, double *C, size_t N)
{
    zero_matrix(C, N);
    for (size_t j = 0; j < N; j++)
        for (size_t i = 0; i < N; i++)
            for (size_t k = 0; k < N; k++)
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
}

static void matmul_jki(const double *A, const double *B, double *C, size_t N)
{
    zero_matrix(C, N);
    for (size_t j = 0; j < N; j++)
        for (size_t k = 0; k < N; k++) {
            double bkj = B[IDX(k, j, N)];
            for (size_t i = 0; i < N; i++)
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * bkj;
        }
}

static void matmul_kij(const double *A, const double *B, double *C, size_t N)
{
    zero_matrix(C, N);
    for (size_t k = 0; k < N; k++)
        for (size_t i = 0; i < N; i++) {
            double aik = A[IDX(i, k, N)];
            for (size_t j = 0; j < N; j++)
                C[IDX(i, j, N)] += aik * B[IDX(k, j, N)];
        }
}

static void matmul_kji(const double *A, const double *B, double *C, size_t N)
{
    zero_matrix(C, N);
    for (size_t k = 0; k < N; k++)
        for (size_t j = 0; j < N; j++) {
            double bkj = B[IDX(k, j, N)];
            for (size_t i = 0; i < N; i++)
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * bkj;
        }
}

static double benchmark(matmul_fn function,
                        const double *A,
                        const double *B,
                        double *C,
                        size_t N,
                        int repetitions)
{
    double best = -1.0;

    for (int r = 0; r < repetitions; r++) {
        double start = seconds();
        function(A, B, C, N);
        double elapsed = seconds() - start;

        if (best < 0.0 || elapsed < best) {
            best = elapsed;
        }
    }

    return best;
}

int main(int argc, char *argv[])
{
    if (argc != 5 && argc != 6) {
        fprintf(stderr, "Usage: %s a b N fileout [repetitions]\n", argv[0]);
        fprintf(stderr, "Example: %s 2 3 200 output.txt 3\n", argv[0]);
        return 1;
    }

    char *end = NULL;
    errno = 0;
    double a = strtod(argv[1], &end);
    if (errno != 0 || end == argv[1] || *end != '\0') {
        fprintf(stderr, "Error: a must be a number.\n");
        return 1;
    }

    errno = 0;
    double b = strtod(argv[2], &end);
    if (errno != 0 || end == argv[2] || *end != '\0') {
        fprintf(stderr, "Error: b must be a number.\n");
        return 1;
    }

    errno = 0;
    unsigned long parsed_N = strtoul(argv[3], &end, 10);
    if (errno != 0 || end == argv[3] || *end != '\0' || parsed_N == 0) {
        fprintf(stderr, "Error: N must be a positive integer.\n");
        return 1;
    }

    const char *fileout = argv[4];
    int repetitions = 3;
    if (argc == 6) {
        errno = 0;
        long parsed_repetitions = strtol(argv[5], &end, 10);
        if (errno != 0 || end == argv[5] || *end != '\0' || parsed_repetitions <= 0) {
            fprintf(stderr, "Error: repetitions must be a positive integer.\n");
            return 1;
        }
        repetitions = (int)parsed_repetitions;
    }

    size_t N = (size_t)parsed_N;
    if (N > ((size_t)-1 / N) || N * N > ((size_t)-1 / sizeof(double))) {
        fprintf(stderr, "Error: N is too large.\n");
        return 1;
    }

    size_t elements = N * N;
    double *A = malloc(elements * sizeof(*A));
    double *B = malloc(elements * sizeof(*B));
    double *C = malloc(elements * sizeof(*C));

    if (A == NULL || B == NULL || C == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }

    fill_matrix(A, N, a);
    fill_matrix(B, N, b);

    double expected = (double)N * a * b;
    printf("N = %zu, a = %.6g, b = %.6g\n", N, a, b);
    printf("Expected value of each C element: %.10g\n\n", expected);

    Variant variants[] = {
        {"ijk", matmul_ijk, 0.0},
        {"ikj", matmul_ikj, 0.0},
        {"jik", matmul_jik, 0.0},
        {"jki", matmul_jki, 0.0},
        {"kij", matmul_kij, 0.0},
        {"kji", matmul_kji, 0.0}
    };

    size_t nvariants = sizeof(variants) / sizeof(variants[0]);
    size_t best_index = 0;

    for (size_t v = 0; v < nvariants; v++) {
        variants[v].time = benchmark(variants[v].function, A, B, C, N, repetitions);
        printf("%s: %.6f s | quick check: %s\n",
               variants[v].name,
               variants[v].time,
               quick_check(C, N, expected) ? "OK" : "FAILED");

        if (variants[v].time < variants[best_index].time || v == 0) {
            best_index = v;
        }
    }

    variants[best_index].function(A, B, C, N);

    printf("\nFastest ordering: %s\n", variants[best_index].name);
    printf("Full check before saving: %s\n",
           full_check(C, N, expected) ? "OK" : "FAILED");

    if (!save_matrix(fileout, C, N)) {
        fprintf(stderr, "Error: could not write output file '%s'.\n", fileout);
        free(A);
        free(B);
        free(C);
        return 1;
    }

    printf("Matrix C saved to %s\n", fileout);

    free(A);
    free(B);
    free(C);

    return 0;
}
