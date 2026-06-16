from decimal import Decimal, getcontext
import csv
import os

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


getcontext().prec = 110

lmax = 50
L = 80
x_values = [0.1, 0.5, 0.9, 0.99]


def forward_double(x):
    P = [0.0] * (lmax + 1)
    P[0] = 1.0
    P[1] = x

    for ell in range(1, lmax):
        P[ell + 1] = (
            ((2.0 * ell + 1.0) / (ell + 1.0)) * x * P[ell]
            - (ell / (ell + 1.0)) * P[ell - 1]
        )

    return P


def reference_decimal(x):
    xd = Decimal(str(x))
    P = [Decimal(0)] * (lmax + 1)
    P[0] = Decimal(1)
    P[1] = xd

    for ell in range(1, lmax):
        P[ell + 1] = (
            (Decimal(2 * ell + 1) / Decimal(ell + 1)) * xd * P[ell]
            - (Decimal(ell) / Decimal(ell + 1)) * P[ell - 1]
        )

    return P


def backward_double(x):
    Q = [0.0] * (L + 2)
    Q[L + 1] = 0.0
    Q[L] = 1.0

    for ell in range(L, 0, -1):
        Q[ell - 1] = (
            ((2.0 * ell + 1.0) / ell) * x * Q[ell]
            - ((ell + 1.0) / ell) * Q[ell + 1]
        )

    scale = 1.0 / Q[0]
    return [scale * Q[ell] for ell in range(lmax + 1)]


rows = []
os.makedirs("plots", exist_ok=True)

for x in x_values:
    P_forward = forward_double(x)
    P_ref = reference_decimal(x)
    P_back = backward_double(x)

    max_forward = Decimal(0)
    max_backward = Decimal(0)

    for ell in range(lmax + 1):
        ref = P_ref[ell]
        fwd = Decimal.from_float(P_forward[ell])
        bwd = Decimal.from_float(P_back[ell])

        abs_err_fwd = abs(fwd - ref)
        abs_err_bwd = abs(bwd - ref)

        if abs(ref) > Decimal("1e-90"):
            rel_err_fwd = abs_err_fwd / abs(ref)
            rel_err_bwd = abs_err_bwd / abs(ref)
        else:
            rel_err_fwd = abs_err_fwd
            rel_err_bwd = abs_err_bwd

        if rel_err_fwd > max_forward:
            max_forward = rel_err_fwd
        if rel_err_bwd > max_backward:
            max_backward = rel_err_bwd

        rows.append(
            [
                x,
                ell,
                str(ref),
                repr(P_forward[ell]),
                repr(P_back[ell]),
                str(abs_err_fwd),
                str(rel_err_fwd),
                str(abs_err_bwd),
                str(rel_err_bwd),
            ]
        )

    print(
        f"x = {x:g}: max relative error forward = {max_forward:.3E}, "
        f"backward = {max_backward:.3E}"
    )


with open("legendre_errors.csv", "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow(
        [
            "x",
            "ell",
            "P_reference",
            "P_forward",
            "P_backward",
            "abs_err_forward",
            "rel_err_forward",
            "abs_err_backward",
            "rel_err_backward",
        ]
    )
    writer.writerows(rows)


def make_plot(column, title, ylabel, filename):
    plt.figure(figsize=(7, 4.5))

    for x in x_values:
        ell_values = []
        err_values = []

        for row in rows:
            if row[0] == x:
                ell_values.append(row[1])
                value = float(Decimal(row[column]))
                if value == 0.0:
                    value = 1e-35
                err_values.append(value)

        plt.plot(ell_values, err_values, marker="o", markersize=3, label=f"x={x:g}")

    plt.yscale("log")
    plt.xlabel("degree ell")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(filename, dpi=180)
    plt.close()


make_plot(6, "Forward recurrence: relative error", "relative error", "plots/forward_relative_error.png")
make_plot(8, "Backward recurrence: relative error", "relative error", "plots/backward_relative_error.png")
make_plot(5, "Forward recurrence: absolute error", "absolute error", "plots/forward_absolute_error.png")
make_plot(7, "Backward recurrence: absolute error", "absolute error", "plots/backward_absolute_error.png")

print("Wrote legendre_errors.csv and plots/*.png")
