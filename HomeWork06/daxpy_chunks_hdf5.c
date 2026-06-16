#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hdf5.h"

#define MAX_LINE 512
#define MAX_PATH 256

typedef struct {
    long N;
    long chunk_size;
    double a;
    double x_value;
    double y_value;
    char output_file[MAX_PATH];
} Config;

static char *trim(char *text)
{
    char *end;

    while (*text == ' ' || *text == '\t' || *text == '\n' || *text == '\r') {
        text++;
    }

    if (*text == '\0') {
        return text;
    }

    end = text + strlen(text) - 1;
    while (end > text &&
           (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }

    return text;
}

static int parse_long(const char *value, long *out)
{
    char *end = NULL;

    errno = 0;
    *out = strtol(value, &end, 10);

    return errno == 0 && end != value && *trim(end) == '\0';
}

static int parse_double(const char *value, double *out)
{
    char *end = NULL;

    errno = 0;
    *out = strtod(value, &end);

    return errno == 0 && end != value && *trim(end) == '\0';
}

static int read_config(const char *path, Config *cfg)
{
    FILE *file = fopen(path, "r");
    char line[MAX_LINE];

    cfg->N = -1;
    cfg->chunk_size = -1;
    cfg->a = 0.0;
    cfg->x_value = 0.0;
    cfg->y_value = 0.0;
    cfg->output_file[0] = '\0';

    if (file == NULL) {
        fprintf(stderr, "Error: cannot open input file '%s'\n", path);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *comment = strchr(line, '#');
        char *equal;
        char *key;
        char *value;

        if (comment != NULL) {
            *comment = '\0';
        }

        equal = strchr(line, '=');
        if (equal == NULL) {
            continue;
        }

        *equal = '\0';
        key = trim(line);
        value = trim(equal + 1);

        if (strcmp(key, "N") == 0) {
            if (!parse_long(value, &cfg->N)) {
                fprintf(stderr, "Error: invalid N\n");
                fclose(file);
                return 0;
            }
        } else if (strcmp(key, "chunk_size") == 0) {
            if (!parse_long(value, &cfg->chunk_size)) {
                fprintf(stderr, "Error: invalid chunk_size\n");
                fclose(file);
                return 0;
            }
        } else if (strcmp(key, "a") == 0) {
            if (!parse_double(value, &cfg->a)) {
                fprintf(stderr, "Error: invalid a\n");
                fclose(file);
                return 0;
            }
        } else if (strcmp(key, "x_value") == 0) {
            if (!parse_double(value, &cfg->x_value)) {
                fprintf(stderr, "Error: invalid x_value\n");
                fclose(file);
                return 0;
            }
        } else if (strcmp(key, "y_value") == 0) {
            if (!parse_double(value, &cfg->y_value)) {
                fprintf(stderr, "Error: invalid y_value\n");
                fclose(file);
                return 0;
            }
        } else if (strcmp(key, "output_file") == 0) {
            strncpy(cfg->output_file, value, MAX_PATH - 1);
            cfg->output_file[MAX_PATH - 1] = '\0';
        }
    }

    fclose(file);

    if (cfg->N <= 0 || cfg->chunk_size <= 0 || cfg->output_file[0] == '\0') {
        fprintf(stderr, "Error: config must define positive N, positive chunk_size, and output_file\n");
        return 0;
    }

    return 1;
}

static int almost_equal(double left, double right)
{
    double scale = fmax(1.0, fmax(fabs(left), fabs(right)));
    return fabs(left - right) <= 1e-12 * scale;
}

static void daxpy_reference(long N, double a, const double *x, const double *y, double *d)
{
    for (long i = 0; i < N; i++) {
        d[i] = a * x[i] + y[i];
    }
}

static void daxpy_chunked(long N,
                          long chunk_size,
                          double a,
                          const double *x,
                          const double *y,
                          double *d,
                          double *partial_sum,
                          long *chunk_start,
                          long *chunk_end,
                          long *chunk_length)
{
    long number_of_chunks = (N + chunk_size - 1) / chunk_size;

    for (long chunk = 0; chunk < number_of_chunks; chunk++) {
        long start = chunk * chunk_size;
        long end = start + chunk_size;
        double local_sum = 0.0;

        if (end > N) {
            end = N;
        }

        chunk_start[chunk] = start;
        chunk_end[chunk] = end - 1;
        chunk_length[chunk] = end - start;

        for (long i = start; i < end; i++) {
            d[i] = a * x[i] + y[i];
            local_sum += d[i];
        }

        partial_sum[chunk] = local_sum;
    }
}

static double sum_vector(const double *data, long n)
{
    double sum = 0.0;

    for (long i = 0; i < n; i++) {
        sum += data[i];
    }

    return sum;
}

static int vectors_match(const double *left, const double *right, long n)
{
    for (long i = 0; i < n; i++) {
        if (!almost_equal(left[i], right[i])) {
            fprintf(stderr, "First mismatch at i=%ld: %.17g vs %.17g\n", i, left[i], right[i]);
            return 0;
        }
    }

    return 1;
}

static void require_hdf5(herr_t status, const char *message)
{
    if (status < 0) {
        fprintf(stderr, "HDF5 error: %s\n", message);
        exit(EXIT_FAILURE);
    }
}

static void write_double_array(hid_t parent, const char *name, const double *data, long n)
{
    hsize_t dims[1] = {(hsize_t)n};
    hid_t space = H5Screate_simple(1, dims, NULL);
    hid_t dataset = H5Dcreate2(parent, name, H5T_NATIVE_DOUBLE, space,
                               H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    if (space < 0 || dataset < 0) {
        fprintf(stderr, "HDF5 error: cannot create dataset '%s'\n", name);
        exit(EXIT_FAILURE);
    }

    require_hdf5(H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                          H5P_DEFAULT, data),
                 "cannot write double array");
    H5Dclose(dataset);
    H5Sclose(space);
}

static void write_long_array(hid_t parent, const char *name, const long *data, long n)
{
    hsize_t dims[1] = {(hsize_t)n};
    hid_t space = H5Screate_simple(1, dims, NULL);
    hid_t dataset = H5Dcreate2(parent, name, H5T_NATIVE_LONG, space,
                               H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    if (space < 0 || dataset < 0) {
        fprintf(stderr, "HDF5 error: cannot create dataset '%s'\n", name);
        exit(EXIT_FAILURE);
    }

    require_hdf5(H5Dwrite(dataset, H5T_NATIVE_LONG, H5S_ALL, H5S_ALL,
                          H5P_DEFAULT, data),
                 "cannot write long array");
    H5Dclose(dataset);
    H5Sclose(space);
}

static void write_double_scalar(hid_t parent, const char *name, double value)
{
    write_double_array(parent, name, &value, 1);
}

static void write_long_scalar(hid_t parent, const char *name, long value)
{
    write_long_array(parent, name, &value, 1);
}

static void save_hdf5(const Config *cfg,
                      long number_of_chunks,
                      const double *x,
                      const double *y,
                      const double *d_reference,
                      const double *d_chunked,
                      const double *partial_sum,
                      const long *chunk_start,
                      const long *chunk_end,
                      const long *chunk_length)
{
    hid_t file = H5Fcreate(cfg->output_file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    hid_t config_group;
    hid_t vectors_group;
    hid_t chunks_group;

    if (file < 0) {
        fprintf(stderr, "HDF5 error: cannot create file '%s'\n", cfg->output_file);
        exit(EXIT_FAILURE);
    }

    config_group = H5Gcreate2(file, "/config", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    vectors_group = H5Gcreate2(file, "/vectors", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    chunks_group = H5Gcreate2(file, "/chunks", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    if (config_group < 0 || vectors_group < 0 || chunks_group < 0) {
        fprintf(stderr, "HDF5 error: cannot create top-level groups\n");
        exit(EXIT_FAILURE);
    }

    write_long_scalar(config_group, "N", cfg->N);
    write_long_scalar(config_group, "chunk_size", cfg->chunk_size);
    write_long_scalar(config_group, "number_of_chunks", number_of_chunks);
    write_double_scalar(config_group, "a", cfg->a);
    write_double_scalar(config_group, "x_value", cfg->x_value);
    write_double_scalar(config_group, "y_value", cfg->y_value);

    write_double_array(vectors_group, "x", x, cfg->N);
    write_double_array(vectors_group, "y", y, cfg->N);
    write_double_array(vectors_group, "d_reference", d_reference, cfg->N);
    write_double_array(vectors_group, "d_chunked", d_chunked, cfg->N);

    write_long_array(chunks_group, "start_index", chunk_start, number_of_chunks);
    write_long_array(chunks_group, "end_index", chunk_end, number_of_chunks);
    write_long_array(chunks_group, "length", chunk_length, number_of_chunks);
    write_double_array(chunks_group, "partial_sum", partial_sum, number_of_chunks);

    for (long chunk = 0; chunk < number_of_chunks; chunk++) {
        char group_name[64];
        hid_t chunk_group;
        long start = chunk_start[chunk];
        long length = chunk_length[chunk];

        snprintf(group_name, sizeof(group_name), "chunk_%04ld", chunk);
        chunk_group = H5Gcreate2(chunks_group, group_name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        if (chunk_group < 0) {
            fprintf(stderr, "HDF5 error: cannot create %s\n", group_name);
            exit(EXIT_FAILURE);
        }

        write_long_scalar(chunk_group, "start_index", chunk_start[chunk]);
        write_long_scalar(chunk_group, "end_index", chunk_end[chunk]);
        write_long_scalar(chunk_group, "length", length);
        write_double_scalar(chunk_group, "partial_sum", partial_sum[chunk]);
        write_double_array(chunk_group, "x", x + start, length);
        write_double_array(chunk_group, "y", y + start, length);
        write_double_array(chunk_group, "d", d_chunked + start, length);

        H5Gclose(chunk_group);
    }

    H5Gclose(chunks_group);
    H5Gclose(vectors_group);
    H5Gclose(config_group);
    H5Fclose(file);
}

int main(int argc, char *argv[])
{
    Config cfg;
    long number_of_chunks;
    double *x = NULL;
    double *y = NULL;
    double *d_reference = NULL;
    double *d_chunked = NULL;
    double *partial_sum = NULL;
    long *chunk_start = NULL;
    long *chunk_end = NULL;
    long *chunk_length = NULL;
    double reference_sum;
    double chunk_sum;
    int ok_vectors;
    int ok_sums;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s input.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!read_config(argv[1], &cfg)) {
        return EXIT_FAILURE;
    }

    number_of_chunks = (cfg.N + cfg.chunk_size - 1) / cfg.chunk_size;

    x = malloc((size_t)cfg.N * sizeof(*x));
    y = malloc((size_t)cfg.N * sizeof(*y));
    d_reference = malloc((size_t)cfg.N * sizeof(*d_reference));
    d_chunked = malloc((size_t)cfg.N * sizeof(*d_chunked));
    partial_sum = malloc((size_t)number_of_chunks * sizeof(*partial_sum));
    chunk_start = malloc((size_t)number_of_chunks * sizeof(*chunk_start));
    chunk_end = malloc((size_t)number_of_chunks * sizeof(*chunk_end));
    chunk_length = malloc((size_t)number_of_chunks * sizeof(*chunk_length));

    if (x == NULL || y == NULL || d_reference == NULL || d_chunked == NULL ||
        partial_sum == NULL || chunk_start == NULL || chunk_end == NULL ||
        chunk_length == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        free(x);
        free(y);
        free(d_reference);
        free(d_chunked);
        free(partial_sum);
        free(chunk_start);
        free(chunk_end);
        free(chunk_length);
        return EXIT_FAILURE;
    }

    for (long i = 0; i < cfg.N; i++) {
        x[i] = cfg.x_value;
        y[i] = cfg.y_value;
    }

    daxpy_reference(cfg.N, cfg.a, x, y, d_reference);
    daxpy_chunked(cfg.N, cfg.chunk_size, cfg.a, x, y, d_chunked, partial_sum,
                  chunk_start, chunk_end, chunk_length);

    reference_sum = sum_vector(d_reference, cfg.N);
    chunk_sum = sum_vector(partial_sum, number_of_chunks);
    ok_vectors = vectors_match(d_reference, d_chunked, cfg.N);
    ok_sums = almost_equal(reference_sum, chunk_sum);

    printf("HomeWork06 - Chunked DAXPY\n");
    printf("N                = %ld\n", cfg.N);
    printf("chunk_size       = %ld\n", cfg.chunk_size);
    printf("number_of_chunks = %ld\n", number_of_chunks);
    printf("a                = %.17g\n", cfg.a);
    printf("x_value          = %.17g\n", cfg.x_value);
    printf("y_value          = %.17g\n", cfg.y_value);
    printf("reference sum    = %.17g\n", reference_sum);
    printf("chunk sum        = %.17g\n", chunk_sum);
    printf("vector check     = %s\n", ok_vectors ? "OK" : "FAILED");
    printf("sum check        = %s\n", ok_sums ? "OK" : "FAILED");

    save_hdf5(&cfg, number_of_chunks, x, y, d_reference, d_chunked,
              partial_sum, chunk_start, chunk_end, chunk_length);

    printf("HDF5 output      = %s\n", cfg.output_file);

    free(x);
    free(y);
    free(d_reference);
    free(d_chunked);
    free(partial_sum);
    free(chunk_start);
    free(chunk_end);
    free(chunk_length);

    return (ok_vectors && ok_sums) ? EXIT_SUCCESS : EXIT_FAILURE;
}
