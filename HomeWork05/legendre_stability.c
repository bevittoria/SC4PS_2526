#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LMAX 50
#define L_BACKWARD 80
#define N_X 4

static const double x_values[N_X] = {0.1, 0.5, 0.9, 0.99};

typedef struct {
    double hi;
    double lo;
} dd_t;

static FILE *open_file(const char *filename)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    return file;
}

static dd_t dd_normalize(double hi, double lo)
{
    dd_t result;
    double sum = hi + lo;
    double err = lo - (sum - hi);

    result.hi = sum;
    result.lo = err;
    return result;
}

static dd_t dd_from_double(double value)
{
    dd_t result = {value, 0.0};

    return result;
}

static dd_t dd_add(dd_t a, dd_t b)
{
    double sum = a.hi + b.hi;
    double v = sum - a.hi;
    double err = (a.hi - (sum - v)) + (b.hi - v) + a.lo + b.lo;

    return dd_normalize(sum, err);
}

static dd_t dd_sub(dd_t a, dd_t b)
{
    dd_t minus_b = {-b.hi, -b.lo};

    return dd_add(a, minus_b);
}

static dd_t dd_mul_double(dd_t a, double b)
{
    double product = a.hi * b;
    double err = fma(a.hi, b, -product) + a.lo * b;

    return dd_normalize(product, err);
}

static double dd_to_double(dd_t a)
{
    return a.hi + a.lo;
}

static double dd_abs(dd_t a)
{
    return fabs(dd_to_double(a));
}

/*
 * Forward Bonnet recurrence in double precision:
 *
 *     P_0(x) = 1
 *     P_1(x) = x
 *     P_{l+1}(x) = ((2l+1)/(l+1)) x P_l(x) - (l/(l+1)) P_{l-1}(x)
 */
static void legendre_forward_double(double x, double p[LMAX + 1])
{
    p[0] = 1.0;
    p[1] = x;

    for (int l = 1; l < LMAX; ++l) {
        p[l + 1] = ((2.0 * l + 1.0) / (l + 1.0)) * x * p[l]
                 - (l / (l + 1.0)) * p[l - 1];
    }
}

/*
 * Forward recurrence in double-double precision.
 * This provides a C reference more accurate than ordinary double arithmetic.
 */
static void legendre_reference_dd(double x, dd_t p[LMAX + 1])
{
    p[0] = dd_from_double(1.0);
    p[1] = dd_from_double(x);

    for (int l = 1; l < LMAX; ++l) {
        double coeff1 = ((2.0 * l + 1.0) / (l + 1.0)) * x;
        double coeff2 = l / (l + 1.0);
        dd_t term1 = dd_mul_double(p[l], coeff1);
        dd_t term2 = dd_mul_double(p[l - 1], coeff2);

        p[l + 1] = dd_sub(term1, term2);
    }
}

/*
 * Backward recurrence experiment.
 *
 * We start from arbitrary values at high degree:
 *
 *     Q_{L+1} = 0
 *     Q_L     = 1
 *
 * Then we recurse downward and rescale so that Q_0 = 1.
 */
static void legendre_backward_double(double x, double p[LMAX + 1])
{
    double q[L_BACKWARD + 2];

    for (int l = 0; l <= L_BACKWARD + 1; ++l) {
        q[l] = 0.0;
    }

    q[L_BACKWARD + 1] = 0.0;
    q[L_BACKWARD] = 1.0;

    for (int l = L_BACKWARD; l > 0; --l) {
        q[l - 1] = ((2.0 * l + 1.0) / l) * x * q[l]
                 - ((l + 1.0) / l) * q[l + 1];
    }

    for (int l = 0; l <= LMAX; ++l) {
        p[l] = q[l] / q[0];
    }
}

static double relative_error(dd_t value, dd_t reference)
{
    double abs_error = dd_abs(dd_sub(value, reference));
    double ref_abs = dd_abs(reference);

    if (ref_abs > 1.0e-90) {
        return abs_error / ref_abs;
    }

    return abs_error;
}

int main(void)
{
    FILE *file = open_file("legendre_errors.csv");

    fprintf(file,
            "x,ell,P_reference,P_forward,P_backward,"
            "abs_err_forward,rel_err_forward,"
            "abs_err_backward,rel_err_backward\n");

    for (int ix = 0; ix < N_X; ++ix) {
        double x = x_values[ix];
        double p_forward[LMAX + 1];
        double p_backward[LMAX + 1];
        dd_t p_reference[LMAX + 1];
        double max_rel_forward = 0.0;
        double max_rel_backward = 0.0;

        legendre_forward_double(x, p_forward);
        legendre_backward_double(x, p_backward);
        legendre_reference_dd(x, p_reference);

        for (int l = 0; l <= LMAX; ++l) {
            dd_t ref = p_reference[l];
            dd_t fwd = dd_from_double(p_forward[l]);
            dd_t bwd = dd_from_double(p_backward[l]);
            double abs_err_forward = dd_abs(dd_sub(fwd, ref));
            double abs_err_backward = dd_abs(dd_sub(bwd, ref));
            double rel_err_forward = relative_error(fwd, ref);
            double rel_err_backward = relative_error(bwd, ref);

            if (rel_err_forward > max_rel_forward) {
                max_rel_forward = rel_err_forward;
            }
            if (rel_err_backward > max_rel_backward) {
                max_rel_backward = rel_err_backward;
            }

            fprintf(file,
                    "%.17g,%d,%.17g,%.17g,%.17g,"
                    "%.17e,%.17e,%.17e,%.17e\n",
                    x, l, dd_to_double(ref), p_forward[l], p_backward[l],
                    abs_err_forward, rel_err_forward,
                    abs_err_backward, rel_err_backward);
        }

        printf("x = %.17g: max relative error forward = %.3e, "
               "backward = %.3e\n",
               x, max_rel_forward, max_rel_backward);
    }

    fclose(file);
    printf("Wrote legendre_errors.csv\n");

    return 0;
}
