import os

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np


os.makedirs("plots", exist_ok=True)


# 1. Coin tosses and LLN
coin = np.loadtxt("coin_tosses.csv", delimiter=",", skiprows=1)

plt.figure(figsize=(7, 4.5))
plt.plot(coin[:, 0], coin[:, 1], label="running fraction of heads")
plt.axhline(0.5, color="red", linestyle="--", label="expected value 0.5")
plt.xscale("log")
plt.xlabel("number of tosses")
plt.ylabel("fraction of heads")
plt.title("Coin tosses and law of large numbers")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("plots/coin_tosses.png", dpi=180)
plt.close()


# 2. Monte Carlo pi
pi_data = np.loadtxt("pi_errors.csv", delimiter=",", skiprows=1)

plt.figure(figsize=(7, 4.5))
plt.plot(pi_data[:, 0], pi_data[:, 2], marker="o")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("N")
plt.ylabel("absolute error")
plt.title("Monte Carlo estimate of pi")
plt.grid(True)
plt.tight_layout()
plt.savefig("plots/pi_error.png", dpi=180)
plt.close()


# 3. Change of variables Y = U^2
y = np.loadtxt("change_variables.csv", delimiter=",", skiprows=1)
x_grid = np.linspace(0.001, 1.0, 500)
pdf = 1.0 / (2.0 * np.sqrt(x_grid))

plt.figure(figsize=(7, 4.5))
plt.hist(y, bins=80, density=True, alpha=0.65, label="simulation")
plt.plot(x_grid, pdf, color="red", label="1/(2 sqrt(y))")
plt.xlabel("y")
plt.ylabel("density")
plt.title("Change of variables: Y = U^2")
plt.ylim(0, 8)
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("plots/change_variables.png", dpi=180)
plt.close()


# 4. Inverse transform exponential
exp_sample = np.loadtxt("exponential.csv", delimiter=",", skiprows=1)
lambda_value = 1.5
x_grid = np.linspace(0.0, np.percentile(exp_sample, 99.5), 500)
pdf = lambda_value * np.exp(-lambda_value * x_grid)

plt.figure(figsize=(7, 4.5))
plt.hist(exp_sample, bins=80, density=True, alpha=0.65, label="simulation")
plt.plot(x_grid, pdf, color="red", label="exact PDF")
plt.xlabel("y")
plt.ylabel("density")
plt.title("Inverse transform exponential")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("plots/exponential_pdf.png", dpi=180)
plt.close()


# 5. Empirical CDF
sorted_sample = np.sort(exp_sample)
empirical_cdf = np.arange(1, len(sorted_sample) + 1) / len(sorted_sample)
exact_cdf = 1.0 - np.exp(-lambda_value * x_grid)

plt.figure(figsize=(7, 4.5))
plt.plot(sorted_sample, empirical_cdf, label="empirical CDF")
plt.plot(x_grid, exact_cdf, color="red", linestyle="--", label="exact CDF")
plt.xlabel("y")
plt.ylabel("CDF")
plt.title("Empirical CDF of exponential sample")
plt.xlim(0, np.percentile(exp_sample, 99.5))
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("plots/empirical_cdf.png", dpi=180)
plt.close()


print("Plots saved in plots/")
