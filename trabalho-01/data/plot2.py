import os
import pandas as pd
import matplotlib.pyplot as plt

input_csv = "resultados_agregados.csv"
output_folder = "graphs_2"
os.makedirs(output_folder, exist_ok=True)

# ---------------------------------------------------------------
# Read
df = pd.read_csv(input_csv)

# Convert numeric columns
df["n"] = pd.to_numeric(df["n"], errors="coerce")
df["mean_f"] = pd.to_numeric(df["mean_f"], errors="coerce")
df["std_f"] = pd.to_numeric(df["std_f"], errors="coerce")

# Remove invalid rows
df = df.dropna(subset=["n", "mean_f", "std_f"])

# ---------------------------------------------------------------
# Colors
algoritmos = sorted(df["algoritmo"].unique())
cmap = plt.get_cmap("tab10")

color_map = {
    alg: cmap(i % 10)
    for i, alg in enumerate(algoritmos)
}

# ---------------------------------------------------------------
# Create chart
for funcao in df["funcao"].unique():

    df_func = df[df["funcao"] == funcao].copy()

    plt.figure(figsize=(10, 6))

    grouped = df_func.groupby(["algoritmo", "metodo", "funcao"])

    for (algoritmo, metodo, funcao_name), group in grouped:

        group = group.sort_values("n")

        color = color_map[algoritmo]

        linestyle = "--" if str(metodo).lower() == "inertia" else "-"

        label = f"{algoritmo} | {metodo}"

        plt.errorbar(
            group["n"],
            group["mean_f"],
            yerr=group["std_f"],
            marker="o",
            linestyle=linestyle,
            color=color,
            capsize=4,
            linewidth=1.8,
            label=label
        )

    # ======================================================
    # Axis's
    plt.yscale("log")
    plt.ylim(1e-4, 1e1/2)

    plt.xlabel("n")
    plt.ylabel("mean_f")
    plt.title(f"Function: {funcao}")

    plt.grid(True, which="both", linestyle="--", alpha=0.5)
    plt.legend(fontsize=8)
    plt.tight_layout()

    output_file = os.path.join(output_folder, f"{funcao}.png")
    plt.savefig(output_file, dpi=300)
    plt.close()

print(f"Charts saved in folder: {output_folder}")