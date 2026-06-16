from decimal import Decimal, getcontext
from pathlib import Path
import csv
import math

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


LMAX = 50
L_BACK = 80
X_VALUES = [0.1, 0.5, 0.9, 0.99]
PRECISION_DIGITS = 110


def legendre_forward_double(x, lmax):
    values = [0.0] * (lmax + 1)
    values[0] = 1.0

    if lmax >= 1:
        values[1] = x

    for ell in range(1, lmax):
        values[ell + 1] = (
            ((2.0 * ell + 1.0) / (ell + 1.0)) * x * values[ell]
            - (ell / (ell + 1.0)) * values[ell - 1]
        )

    return values


def legendre_reference_decimal(x_string, lmax):
    x = Decimal(x_string)
    values = [Decimal(0)] * (lmax + 1)
    values[0] = Decimal(1)

    if lmax >= 1:
        values[1] = x

    for ell in range(1, lmax):
        ell_d = Decimal(ell)
        values[ell + 1] = (
            (Decimal(2 * ell + 1) / Decimal(ell + 1)) * x * values[ell]
            - (ell_d / Decimal(ell + 1)) * values[ell - 1]
        )

    return values


def legendre_backward_double(x, lmax, start_l):
    if start_l <= lmax:
        raise ValueError("start_l must be larger than lmax")

    q = [0.0] * (start_l + 2)
    q[start_l + 1] = 0.0
    q[start_l] = 1.0

    for ell in range(start_l, 0, -1):
        q[ell - 1] = (
            ((2.0 * ell + 1.0) / ell) * x * q[ell]
            - ((ell + 1.0) / ell) * q[ell + 1]
        )

    scale = 1.0 / q[0]
    return [scale * q[ell] for ell in range(lmax + 1)]


def decimal_abs(value):
    return value.copy_abs()


def errors(numerical, reference):
    value = Decimal.from_float(numerical)
    abs_err = decimal_abs(value - reference)
    if decimal_abs(reference) > Decimal("1e-90"):
        rel_err = abs_err / decimal_abs(reference)
    else:
        rel_err = abs_err
    return abs_err, rel_err


def as_float_for_plot(value):
    numeric = float(value)
    if numeric == 0.0:
        return 1e-35
    return numeric


def compute_rows():
    getcontext().prec = PRECISION_DIGITS
    rows = []

    for x in X_VALUES:
        x_string = format(x, ".17g")
        reference = legendre_reference_decimal(x_string, LMAX)
        forward = legendre_forward_double(x, LMAX)
        backward = legendre_backward_double(x, LMAX, L_BACK)

        for ell in range(LMAX + 1):
            abs_f, rel_f = errors(forward[ell], reference[ell])
            abs_b, rel_b = errors(backward[ell], reference[ell])

            rows.append(
                {
                    "x": x,
                    "ell": ell,
                    "P_reference": str(reference[ell]),
                    "P_forward": format(forward[ell], ".17g"),
                    "P_backward": format(backward[ell], ".17g"),
                    "abs_err_forward": str(abs_f),
                    "rel_err_forward": str(rel_f),
                    "abs_err_backward": str(abs_b),
                    "rel_err_backward": str(rel_b),
                }
            )

    return rows


def write_csv(rows, path):
    fieldnames = [
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

    with path.open("w", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def plot_error(rows, column, ylabel, title, path):
    plt.figure(figsize=(7, 4.5))

    for x in X_VALUES:
        selected = [row for row in rows if math.isclose(row["x"], x)]
        ell = [row["ell"] for row in selected]
        err = [as_float_for_plot(Decimal(row[column])) for row in selected]
        plt.plot(ell, err, marker="o", markersize=3, linewidth=1, label=f"x = {x:g}")

    plt.yscale("log")
    plt.xlabel("degree ell")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(True, which="both", alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(path, dpi=180)
    plt.close()


def print_summary(rows):
    print("HomeWork05 - Stability of Legendre Polynomials")
    print(f"lmax = {LMAX}, backward start L = {L_BACK}")
    print()

    for x in X_VALUES:
        selected = [row for row in rows if math.isclose(row["x"], x)]
        max_forward = max(Decimal(row["rel_err_forward"]) for row in selected)
        max_backward = max(Decimal(row["rel_err_backward"]) for row in selected)
        print(
            f"x = {x:g}: max rel error forward = {max_forward:.3E}, "
            f"backward = {max_backward:.3E}"
        )


def main():
    base = Path(__file__).resolve().parent
    plots_dir = base / "plots"
    plots_dir.mkdir(exist_ok=True)

    rows = compute_rows()
    write_csv(rows, base / "legendre_errors.csv")

    plot_error(
        rows,
        "rel_err_forward",
        "relative error",
        "Forward recurrence: relative error",
        plots_dir / "forward_relative_error.png",
    )
    plot_error(
        rows,
        "rel_err_backward",
        "relative error",
        "Backward recurrence experiment: relative error",
        plots_dir / "backward_relative_error.png",
    )
    plot_error(
        rows,
        "abs_err_forward",
        "absolute error",
        "Forward recurrence: absolute error",
        plots_dir / "forward_absolute_error.png",
    )
    plot_error(
        rows,
        "abs_err_backward",
        "absolute error",
        "Backward recurrence experiment: absolute error",
        plots_dir / "backward_absolute_error.png",
    )

    print_summary(rows)
    print()
    print("Wrote legendre_errors.csv and plots/*.png")


if __name__ == "__main__":
    main()
