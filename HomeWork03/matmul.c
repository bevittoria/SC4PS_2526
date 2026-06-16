#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define IDX(i, j, N) ((i) * (N) + (j))

void fill_matrix(double *M, int N, double value)
{
    for (int i = 0; i < N * N; i++) {
        M[i] = value;
    }
}

void zero_matrix(double *M, int N)
{
    for (int i = 0; i < N * N; i++) {
        M[i] = 0.0;
    }
}

double get_time(void)
{
    return (double)clock() / CLOCKS_PER_SEC;
}

double absolute_value(double x)
{
    if (x < 0.0) {
        return -x;
    }
    return x;
}

void matmul_ijk(double *A, double *B, double *C, int N)
{
    zero_matrix(C, N);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
            }
        }
    }
}

void matmul_ikj(double *A, double *B, double *C, int N)
{
    zero_matrix(C, N);

    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            for (int j = 0; j < N; j++) {
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
            }
        }
    }
}

int check_matrix(double *C, int N, double expected)
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (absolute_value(C[IDX(i, j, N)] - expected) > 1e-9) {
                printf("Wrong value at C[%d][%d] = %.10g, expected %.10g\n",
                       i, j, C[IDX(i, j, N)], expected);
                return 0;
            }
        }
    }

    return 1;
}

void save_matrix(const char *filename, double *C, int N)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Error: cannot open output file.\n");
        return;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(file, "%.10g ", C[IDX(i, j, N)]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        printf("Usage: %s a b N fileout\n", argv[0]);
        printf("Example: %s 2 3 4 output.txt\n", argv[0]);
        return 1;
    }

    double a = atof(argv[1]);
    double b = atof(argv[2]);
    int N = atoi(argv[3]);
    const char *fileout = argv[4];

    if (N <= 0) {
        printf("Error: N must be positive.\n");
        return 1;
    }

    double *A = malloc(N * N * sizeof(double));
    double *B = malloc(N * N * sizeof(double));
    double *C_ijk = malloc(N * N * sizeof(double));
    double *C_ikj = malloc(N * N * sizeof(double));

    if (A == NULL || B == NULL || C_ijk == NULL || C_ikj == NULL) {
        printf("Error: memory allocation failed.\n");
        free(A);
        free(B);
        free(C_ijk);
        free(C_ikj);
        return 1;
    }

    fill_matrix(A, N, a);
    fill_matrix(B, N, b);

    double expected = N * a * b;

    double start = get_time();
    matmul_ijk(A, B, C_ijk, N);
    double time_ijk = get_time() - start;

    start = get_time();
    matmul_ikj(A, B, C_ikj, N);
    double time_ikj = get_time() - start;

    printf("Expected value of each C element = %.10g\n", expected);
    printf("ijk time = %.6f seconds | check: %s\n",
           time_ijk, check_matrix(C_ijk, N, expected) ? "OK" : "FAILED");
    printf("ikj time = %.6f seconds | check: %s\n",
           time_ikj, check_matrix(C_ikj, N, expected) ? "OK" : "FAILED");

    if (time_ikj < time_ijk) {
        printf("Fastest ordering: ikj\n");
        save_matrix(fileout, C_ikj, N);
    } else {
        printf("Fastest ordering: ijk\n");
        save_matrix(fileout, C_ijk, N);
    }

    printf("Matrix saved to %s\n", fileout);

    free(A);
    free(B);
    free(C_ijk);
    free(C_ikj);

    return 0;
}
