#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hdf5.h"

void trim(char *s)
{
    int n = (int)strlen(s);

    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\n' ||
                     s[n - 1] == '\t' || s[n - 1] == '\r')) {
        s[n - 1] = '\0';
        n--;
    }

    while (*s == ' ' || *s == '\t') {
        memmove(s, s + 1, strlen(s));
    }
}

void read_input(const char *filename,
                long *N,
                long *chunk_size,
                double *a,
                double *x_value,
                double *y_value,
                char *output_file)
{
    FILE *file = fopen(filename, "r");
    char line[256];

    if (file == NULL) {
        printf("Error: cannot open input file %s\n", filename);
        exit(1);
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char key[128];
        char value[128];
        char *equal;

        if (line[0] == '#' || strlen(line) < 3) {
            continue;
        }

        equal = strchr(line, '=');
        if (equal == NULL) {
            continue;
        }

        *equal = '\0';
        strcpy(key, line);
        strcpy(value, equal + 1);

        trim(key);
        trim(value);

        if (strcmp(key, "N") == 0) {
            *N = atol(value);
        } else if (strcmp(key, "chunk_size") == 0) {
            *chunk_size = atol(value);
        } else if (strcmp(key, "a") == 0) {
            *a = atof(value);
        } else if (strcmp(key, "x_value") == 0) {
            *x_value = atof(value);
        } else if (strcmp(key, "y_value") == 0) {
            *y_value = atof(value);
        } else if (strcmp(key, "output_file") == 0) {
            strcpy(output_file, value);
        }
    }

    fclose(file);
}

void write_double_array(hid_t file, const char *name, double *data, long n)
{
    hsize_t dims[1] = {(hsize_t)n};
    hid_t space = H5Screate_simple(1, dims, NULL);
    hid_t dataset = H5Dcreate2(file, name, H5T_NATIVE_DOUBLE, space,
                               H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    H5Dclose(dataset);
    H5Sclose(space);
}

void write_long_array(hid_t file, const char *name, long *data, long n)
{
    hsize_t dims[1] = {(hsize_t)n};
    hid_t space = H5Screate_simple(1, dims, NULL);
    hid_t dataset = H5Dcreate2(file, name, H5T_NATIVE_LONG, space,
                               H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    H5Dwrite(dataset, H5T_NATIVE_LONG, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    H5Dclose(dataset);
    H5Sclose(space);
}

void write_one_double(hid_t file, const char *name, double value)
{
    write_double_array(file, name, &value, 1);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s daxpy_input.txt\n", argv[0]);
        return 1;
    }

    long N = 0;
    long chunk_size = 0;
    double a = 0.0;
    double x_value = 0.0;
    double y_value = 0.0;
    char output_file[128] = "daxpy_chunks.h5";

    read_input(argv[1], &N, &chunk_size, &a, &x_value, &y_value, output_file);

    if (N <= 0 || chunk_size <= 0) {
        printf("Error: N and chunk_size must be positive.\n");
        return 1;
    }

    long number_of_chunks = (N + chunk_size - 1) / chunk_size;

    double *x = malloc(N * sizeof(double));
    double *y = malloc(N * sizeof(double));
    double *d_original = malloc(N * sizeof(double));
    double *d_chunked = malloc(N * sizeof(double));
    double *partial_sum = malloc(number_of_chunks * sizeof(double));
    long *chunk_start = malloc(number_of_chunks * sizeof(long));
    long *chunk_end = malloc(number_of_chunks * sizeof(long));

    if (x == NULL || y == NULL || d_original == NULL || d_chunked == NULL ||
        partial_sum == NULL || chunk_start == NULL || chunk_end == NULL) {
        printf("Error: memory allocation failed.\n");
        return 1;
    }

    for (long i = 0; i < N; i++) {
        x[i] = x_value;
        y[i] = y_value;
    }

    for (long i = 0; i < N; i++) {
        d_original[i] = a * x[i] + y[i];
    }

    for (long c = 0; c < number_of_chunks; c++) {
        long start = c * chunk_size;
        long end = start + chunk_size;

        if (end > N) {
            end = N;
        }

        chunk_start[c] = start;
        chunk_end[c] = end - 1;
        partial_sum[c] = 0.0;

        for (long i = start; i < end; i++) {
            d_chunked[i] = a * x[i] + y[i];
            partial_sum[c] += d_chunked[i];
        }
    }

    int vectors_ok = 1;
    for (long i = 0; i < N; i++) {
        if (fabs(d_original[i] - d_chunked[i]) > 1e-12) {
            vectors_ok = 0;
            break;
        }
    }

    double sum_original = 0.0;
    for (long i = 0; i < N; i++) {
        sum_original += d_original[i];
    }

    double sum_chunks = 0.0;
    for (long c = 0; c < number_of_chunks; c++) {
        sum_chunks += partial_sum[c];
    }

    double sum_difference = fabs(sum_original - sum_chunks);
    double sum_scale = fmax(fabs(sum_original), fabs(sum_chunks));
    double sum_tolerance = 1e-12 * fmax(1.0, sum_scale);
    int sums_ok = sum_difference <= sum_tolerance;

    printf("N = %ld\n", N);
    printf("chunk_size = %ld\n", chunk_size);
    printf("number_of_chunks = %ld\n", number_of_chunks);
    printf("vector check: %s\n", vectors_ok ? "OK" : "FAILED");
    printf("sum original = %.17g\n", sum_original);
    printf("sum chunks   = %.17g\n", sum_chunks);
    printf("sum difference = %.17g\n", sum_difference);
    printf("sum tolerance  = %.17g\n", sum_tolerance);
    printf("sum check: %s\n", sums_ok ? "OK" : "FAILED");

    hid_t file = H5Fcreate(output_file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    write_double_array(file, "/x", x, N);
    write_double_array(file, "/y", y, N);
    write_double_array(file, "/d_original", d_original, N);
    write_double_array(file, "/d_chunked", d_chunked, N);
    write_double_array(file, "/partial_sum", partial_sum, number_of_chunks);
    write_long_array(file, "/chunk_start", chunk_start, number_of_chunks);
    write_long_array(file, "/chunk_end", chunk_end, number_of_chunks);
    write_one_double(file, "/sum_original", sum_original);
    write_one_double(file, "/sum_chunks", sum_chunks);

    for (long c = 0; c < number_of_chunks; c++) {
        char name[64];
        long start = chunk_start[c];
        long length = chunk_end[c] - chunk_start[c] + 1;

        sprintf(name, "/chunk_%04ld", c);
        write_double_array(file, name, d_chunked + start, length);
    }

    H5Fclose(file);

    printf("HDF5 file written: %s\n", output_file);

    free(x);
    free(y);
    free(d_original);
    free(d_chunked);
    free(partial_sum);
    free(chunk_start);
    free(chunk_end);

    return 0;
}
