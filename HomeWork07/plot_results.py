import os

# Keep Matplotlib's cache local to this homework folder. This avoids warnings
# when the default user-level cache directory is not writable.
os.environ.setdefault("MPLCONFIGDIR", os.path.join(os.getcwd(), ".matplotlib"))

import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt
import numpy as np


PLOTS_DIR = "plots"
LAMBDA_VALUE = 1.5


def save_current_figure(filename):
    os.makedirs(PLOTS_DIR, exist_ok=True)
    plt.tight_layout()
    plt.savefig(os.path.join(PLOTS_DIR, filename), dpi=180)
    plt.close()


def plot_coin_tosses():
    coin = np.loadtxt("coin_tosses.csv", delimiter=",", skiprows=1)

    plt.figure(figsize=(7, 4.5))
    plt.plot(coin[:, 0], coin[:, 1], color="#4C78A8",
             label="running fraction of heads")
    plt.axhline(0.5, color="#E45756", linestyle="--",
                label="expected value 0.5")
    plt.xscale("log")
    plt.xlabel("number of tosses")
    plt.ylabel("fraction of heads")
    plt.title("Coin tosses and law of large numbers")
    plt.grid(True)
    plt.legend()
    save_current_figure("coin_tosses.png")


def plot_pi_error():
    pi_data = np.loadtxt("pi_errors.csv", delimiter=",", skiprows=1)

    plt.figure(figsize=(7, 4.5))
    plt.plot(pi_data[:, 0], pi_data[:, 2], marker="o", color="#4C78A8")
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("N")
    plt.ylabel("absolute error")
    plt.title("Monte Carlo estimate of pi")
    plt.grid(True)
    save_current_figure("pi_error.png")


def plot_change_of_variables():
    y = np.loadtxt("change_variables.csv", delimiter=",", skiprows=1)
    grid = np.linspace(0.001, 1.0, 500)
    pdf = 1.0 / (2.0 * np.sqrt(grid))

    plt.figure(figsize=(7, 4.5))
    plt.hist(y, bins=80, density=True, alpha=0.65, color="#4C78A8",
             label="simulation")
    plt.plot(grid, pdf, color="#E45756", label="1/(2 sqrt(y))")
    plt.xlabel("y")
    plt.ylabel("density")
    plt.title("Change of variables: Y = U^2")
    plt.ylim(0, 8)
    plt.grid(True)
    plt.legend()
    save_current_figure("change_variables.png")


def plot_exponential_pdf():
    sample = np.loadtxt("exponential.csv", delimiter=",", skiprows=1)
    grid = np.linspace(0.0, np.percentile(sample, 99.5), 500)
    pdf = LAMBDA_VALUE * np.exp(-LAMBDA_VALUE * grid)

    plt.figure(figsize=(7, 4.5))
    plt.hist(sample, bins=80, density=True, alpha=0.65, color="#72B7B2",
             label="simulation")
    plt.plot(grid, pdf, color="#E45756", label="exact PDF")
    plt.xlabel("y")
    plt.ylabel("density")
    plt.title("Inverse transform exponential")
    plt.grid(True)
    plt.legend()
    save_current_figure("exponential_pdf.png")


def plot_empirical_cdf():
    ecdf = np.loadtxt("exponential_ecdf.csv", delimiter=",", skiprows=1)
    x_max = np.percentile(ecdf[:, 0], 99.5)
    grid = np.linspace(0.0, x_max, 500)
    exact_cdf = 1.0 - np.exp(-LAMBDA_VALUE * grid)

    plt.figure(figsize=(7, 4.5))
    plt.plot(ecdf[:, 0], ecdf[:, 1], color="#4C78A8",
             label="empirical CDF")
    plt.plot(grid, exact_cdf, color="#E45756", linestyle="--",
             label="exact CDF")
    plt.xlim(0.0, x_max)
    plt.xlabel("y")
    plt.ylabel("CDF")
    plt.title("Empirical CDF of exponential sample")
    plt.grid(True)
    plt.legend()
    save_current_figure("empirical_cdf.png")


def main():
    plot_coin_tosses()
    plot_pi_error()
    plot_change_of_variables()
    plot_exponential_pdf()
    plot_empirical_cdf()
    print(f"Plots saved in {PLOTS_DIR}/")


if __name__ == "__main__":
    main()
