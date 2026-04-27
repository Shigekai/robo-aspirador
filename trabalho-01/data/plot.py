import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

csv_file = "resultados_agregados.csv"   # input
output_folder = "graphs_1"               # folder

os.makedirs(output_folder, exist_ok=True)


# ---------------------------------------------
df = pd.read_csv(csv_file)

# Ensure numeric
df["n"] = pd.to_numeric(df["n"], errors="coerce")
df["mean_f"] = pd.to_numeric(df["mean_f"], errors="coerce")
df["std_f"] = pd.to_numeric(df["std_f"], errors="coerce")

# Remove invalid values
df = df.dropna(subset=["n", "mean_f", "std_f"])

# ---------------------------------------------
# Identify columns
col1 = df.columns[0]
col2 = df.columns[1]

print(f"Grouping by columns: {col1}, {col2}")

# ---------------------------------------------
# Style
sns.set(style="whitegrid")

# ---------------------------------------------
# Create for each combination
grouped = df.groupby([col1, col2])

for (val1, val2), group in grouped:
    plt.figure(figsize=(10, 6))

    for func_name, subg in group.groupby("funcao"):
        subg = subg.sort_values("n")

        plt.errorbar(
            subg["n"],
            subg["mean_f"],
            yerr=subg["std_f"],
            marker="o",
            linestyle="-",
            capsize=4,
            label=str(func_name)
        )

    plt.ylim(-0.5, 1.5)
    plt.xlabel("n")
    plt.ylabel("mean_f")
    plt.title(f"{col1}={val1} | {col2}={val2}")
    plt.legend(title="funcao")
    plt.tight_layout()

    filename = f"{col1}_{val1}_{col2}_{val2}.png"
    filename = filename.replace(" ", "_").replace("/", "_")
    filepath = os.path.join(output_folder, filename)

    plt.savefig(filepath, dpi=300)
    plt.close()

print(f"Graphs saved in folder: {output_folder}")