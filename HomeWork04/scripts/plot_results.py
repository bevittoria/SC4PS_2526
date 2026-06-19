import os

os.environ.setdefault("MPLCONFIGDIR", os.path.join(os.getcwd(), ".matplotlib"))

import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt
import numpy as np


OUTPUT_DIR = "output"
PLOTS_DIR = "plots"


def load_csv(name):
    return np.loadtxt(os.path.join(OUTPUT_DIR, name), delimiter=",", skiprows=1)


def savefig(name):
    os.makedirs(PLOTS_DIR, exist_ok=True)
    plt.tight_layout()
    plt.savefig(os.path.join(PLOTS_DIR, name), dpi=180)
    plt.close()


def plot_sampling_case(prefix, title, max_freq=None):
    signal = load_csv(f"{prefix}_signal.csv")
    spectrum = load_csv(f"{prefix}_spectrum.csv")

    fig, ax = plt.subplots(1, 2, figsize=(11, 4.2))

    ax[0].plot(signal[:, 0], signal[:, 1], color="#4C78A8", linewidth=1.0)
    ax[0].set_title(f"{title}: signal")
    ax[0].set_xlabel("time [s]")
    ax[0].set_ylabel("amplitude")
    ax[0].grid(True)

    ax[1].plot(spectrum[:, 0], spectrum[:, 1], color="#E45756", linewidth=1.2)
    ax[1].set_title(f"{title}: spectrum")
    ax[1].set_xlabel("frequency [Hz]")
    ax[1].set_ylabel("amplitude")
    ax[1].grid(True)

    if max_freq is not None:
        ax[1].set_xlim(0, max_freq)

    savefig(f"{prefix}.png")


def plot_sampling_summary():
    cases = [
        ("good_sampling", "good sampling", 160),
        ("undersampled", "undersampled", 70),
        ("short_record", "short record", 100),
    ]

    fig, axes = plt.subplots(3, 1, figsize=(8, 8.5), sharex=False)

    for ax, (prefix, title, max_freq) in zip(axes, cases):
        spectrum = load_csv(f"{prefix}_spectrum.csv")
        ax.plot(spectrum[:, 0], spectrum[:, 1], color="#4C78A8", linewidth=1.2)
        ax.set_xlim(0, max_freq)
        ax.set_ylabel("amplitude")
        ax.set_title(title)
        ax.grid(True)

    axes[-1].set_xlabel("frequency [Hz]")
    savefig("sampling_spectra_summary.png")


def plot_coupled_oscillators():
    time = load_csv("coupled_oscillators_time.csv")
    spectrum = load_csv("coupled_oscillators_spectrum.csv")
    f_in = np.sqrt(25.0) / (2.0 * np.pi)
    f_out = np.sqrt(25.0 + 2.0 * 7.0) / (2.0 * np.pi)

    fig, ax = plt.subplots(1, 2, figsize=(11, 4.2))

    ax[0].plot(time[:, 0], time[:, 1], label="x1", color="#4C78A8")
    ax[0].plot(time[:, 0], time[:, 2], label="x2", color="#72B7B2")
    ax[0].set_xlim(0, 20)
    ax[0].set_title("coupled oscillators: time signal")
    ax[0].set_xlabel("time [s]")
    ax[0].set_ylabel("position")
    ax[0].grid(True)
    ax[0].legend()

    ax[1].plot(spectrum[:, 0], spectrum[:, 1], color="#E45756")
    ax[1].axvline(f_in, color="#54A24B", linestyle="--", label="in-phase")
    ax[1].axvline(f_out, color="#B279A2", linestyle="--", label="out-of-phase")
    ax[1].set_xlim(0, 2)
    ax[1].set_title("coupled oscillators: FFT")
    ax[1].set_xlabel("frequency [Hz]")
    ax[1].set_ylabel("amplitude")
    ax[1].grid(True)
    ax[1].legend()

    savefig("coupled_oscillators.png")


def main():
    plot_sampling_case("good_sampling", "Good sampling", 160)
    plot_sampling_case("undersampled", "Undersampled", 70)
    plot_sampling_case("short_record", "Short record", 100)
    plot_sampling_summary()
    plot_coupled_oscillators()
    print(f"Plots written in {PLOTS_DIR}/")


if __name__ == "__main__":
    main()
