import os

import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    csv_in = f"{script_dir}/benchmark_results.csv"

    if not os.path.exists(csv_in):
        print(f"Error: CSV data file not found at {csv_in}")
        return

    df = pd.read_csv(csv_in)

    for bt in df["BuildType"].unique():
        bt_df = df[df["BuildType"] == bt]
        unique_ops = bt_df["Operation"].unique()

        fig, axes = plt.subplots(
            len(unique_ops), 2, figsize=(16, 6 * len(unique_ops)), squeeze=False
        )

        variant_order = ["Reference", "Base", "AVX2", "AVX-512"]
        active_variants = [v for v in variant_order if v in bt_df["Variant"].unique()]
        impl_variants = [v for v in active_variants if v != "Reference"]

        for idx, op in enumerate(unique_ops):
            op_df = bt_df[bt_df["Operation"] == op]

            ref_data = op_df[op_df["Variant"] == "Reference"]["Value"]
            ref_mean = ref_data.mean() if not ref_data.empty else 0
            ref_std = ref_data.std() if not ref_data.empty else 0

            impl_df = op_df[op_df["Variant"] != "Reference"]

            ax_bar = axes[idx, 0]
            if not impl_df.empty:
                sns.barplot(
                    data=impl_df,
                    x="Variant",
                    y="Value",
                    ax=ax_bar,
                    order=impl_variants,
                    errorbar="sd",
                    hue="Variant",
                    palette="Blues_d",
                    legend=False,
                )
            if not ref_data.empty:
                ax_bar.axhline(
                    ref_mean,
                    color="red",
                    linestyle="--",
                    linewidth=1.5,
                    label="Reference Mean",
                )
                ax_bar.axhspan(
                    ref_mean - ref_std,
                    ref_mean + ref_std,
                    color="red",
                    alpha=0.15,
                    hatch="//",
                    label="Reference StdDev",
                )
            ax_bar.set_title(f"{bt} {op} - Variants vs Reference Baseline")
            ax_bar.set_ylabel("Execution Time (ns)")
            ax_bar.set_xlabel("Variant")
            ax_bar.legend()

            ax_box = axes[idx, 1]
            sns.boxplot(
                data=op_df,
                x="Variant",
                y="Value",
                ax=ax_box,
                order=active_variants,
                hue="Variant",
                palette="Set2",
                legend=False,
            )
            ax_box.set_title(f"{bt} {op} - Full Performance Distribution")
            ax_box.set_ylabel("Execution Time (ns)")
            ax_box.set_xlabel("Variant")

        plt.tight_layout()
        plot_out = f"{script_dir}/benchmark_plots_{bt}.png"
        plt.savefig(plot_out, dpi=300)
        print(f"Saved plots to {plot_out}")
        plt.close()


if __name__ == "__main__":
    main()
