import os

os.environ.setdefault("MPLCONFIGDIR", os.path.join(os.getcwd(), ".mplconfig"))

import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt
import numpy as np


PLOTS_DIR = "plots"
X_VALUES = [0.1, 0.5, 0.9, 0.99]


def load_data():
    return np.genfromtxt("legendre_errors.csv", delimiter=",", names=True)


def savefig(filename):
    os.makedirs(PLOTS_DIR, exist_ok=True)
    plt.tight_layout()
    plt.savefig(os.path.join(PLOTS_DIR, filename), dpi=180)
    plt.close()


def make_plot(data, column, title, ylabel, filename, ylim=None):
    plt.figure(figsize=(7, 4.5))

    for x in X_VALUES:
        mask = np.isclose(data["x"], x)
        ell = data["ell"][mask]
        values = data[column][mask]
        values = np.where(values == 0.0, 1e-35, values)

        plt.plot(ell, values, marker="o", markersize=3, label=f"x={x:g}")

    plt.yscale("log")
    if ylim is not None:
        plt.ylim(ylim)
    plt.xlabel("degree ell")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(True)
    plt.legend()
    savefig(filename)


def main():
    data = load_data()

    make_plot(
        data,
        "rel_err_forward",
        "Forward recurrence: relative error",
        "relative error",
        "forward_relative_error.png",
    )
    make_plot(
        data,
        "rel_err_backward",
        "Backward recurrence: relative error",
        "relative error",
        "backward_relative_error.png",
    )
    make_plot(
        data,
        "abs_err_forward",
        "Forward recurrence: absolute error",
        "absolute error",
        "forward_absolute_error.png",
    )
    make_plot(
        data,
        "abs_err_backward",
        "Backward recurrence: absolute error",
        "absolute error",
        "backward_absolute_error.png",
    )
    make_plot(
        data,
        "rel_err_forward",
        "Forward recurrence: relative error zoom",
        "relative error",
        "forward_relative_error_zoom.png",
        ylim=(1e-19, 1e-12),
    )
    make_plot(
        data,
        "abs_err_forward",
        "Forward recurrence: absolute error zoom",
        "absolute error",
        "forward_absolute_error_zoom.png",
        ylim=(1e-20, 1e-14),
    )
    make_plot(
        data,
        "rel_err_backward",
        "Backward recurrence: relative error zoom",
        "relative error",
        "backward_relative_error_zoom.png",
        ylim=(1e-2, 1e3),
    )
    make_plot(
        data,
        "abs_err_backward",
        "Backward recurrence: absolute error zoom",
        "absolute error",
        "backward_absolute_error_zoom.png",
        ylim=(1e-4, 1e1),
    )

    print(f"Plots written in {PLOTS_DIR}/")


if __name__ == "__main__":
    main()
