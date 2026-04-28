import pandas as pd
import matplotlib.pyplot as plt
import argparse
import sys

def main():
    parser = argparse.ArgumentParser(description="Generate Optimization Trade-off Graph from benchmark CSV.")
    parser.add_argument("csv_file", help="Path to the benchmark CSV file")
    parser.add_argument("--n_target", type=int, help="Specific N to plot", default=None)
    args = parser.parse_args()

    try:
        df = pd.read_csv(args.csv_file)
    except FileNotFoundError:
        print(f"Error: Could not find file {args.csv_file}")
        sys.exit(1)

    # Automatically find the best N (highest N that has multiple Thetas tested)
    if args.n_target is None:
        counts = df.groupby('N')['Theta'].nunique()
        n_target = counts[counts > 1].index.max()
        if pd.isna(n_target):
            print("Error: No body count (N) has multiple Theta values to compare.")
            sys.exit(1)
    else:
        n_target = args.n_target

    subset = df[df['N'] == n_target].copy()
    if subset.empty:
        print(f"Error: No data found for N = {n_target}")
        sys.exit(1)

    # Sort by Theta to make the X-axis ordered properly
    subset = subset.sort_values(by='Theta')

    # Calculate Absolute Energy Error (%)
    # Formula: |(Final - Initial) / Initial| * 100
    subset['EnergyError'] = (abs(subset['FinalTotalEnergy'] - subset['InitialTotalEnergy']) / abs(subset['InitialTotalEnergy'])) * 100

    fig, ax1 = plt.subplots(figsize=(10, 6))

    # Left Y-Axis: Bar chart for Execution Time
    color_time = '#1f77b4' # Professional Blue
    ax1.set_xlabel(r'Theta ($\theta$) Value', fontsize=12, fontweight='bold')
    ax1.set_ylabel('Execution Time (ms)', color=color_time, fontsize=12, fontweight='bold')
    ax1.bar(subset['Theta'].astype(str), subset['AvgTimeMs'], color=color_time, alpha=0.8, width=0.5)
    ax1.tick_params(axis='y', labelcolor=color_time)

    # Right Y-Axis: Line graph for Energy Error overlay
    ax2 = ax1.twinx()
    color_error = '#d62728' # Professional Red
    ax2.set_ylabel('Energy Error Drift (%)', color=color_error, fontsize=12, fontweight='bold')
    ax2.plot(subset['Theta'].astype(str), subset['EnergyError'], color=color_error, marker='o', linewidth=3.5, markersize=9)
    ax2.tick_params(axis='y', labelcolor=color_error)

    plt.title(f'Performance vs. Accuracy Trade-off (N = {n_target})', fontsize=16, fontweight='bold', pad=15)
    
    fig.tight_layout()
    output_filename = f'Graph2_Tradeoff_N{n_target}.png'
    plt.savefig(output_filename, dpi=300)
    print(f"Successfully generated {output_filename}")

if __name__ == "__main__":
    main()