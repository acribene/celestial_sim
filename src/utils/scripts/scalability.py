import pandas as pd
import matplotlib.pyplot as plt
import argparse
import sys

def main():
    parser = argparse.ArgumentParser(description="Generate Scalability Curve from benchmark CSV.")
    parser.add_argument("csv_file", help="Path to the benchmark CSV file")
    args = parser.parse_args()

    try:
        df = pd.read_csv(args.csv_file)
    except FileNotFoundError:
        print(f"Error: Could not find file {args.csv_file}")
        sys.exit(1)

    fig, ax = plt.subplots(figsize=(10, 6))

    # Dynamically plot a line for each Theta value in the CSV
    for theta in sorted(df['Theta'].unique()):
        subset = df[df['Theta'] == theta]
        ax.plot(subset['N'], subset['AvgTimeMs'], marker='o', linewidth=2.5, markersize=6, label=f'Theta = {theta}')

    ax.set_title('Algorithmic Scalability (N vs Execution Time)', fontsize=16, fontweight='bold', pad=15)
    ax.set_xlabel('Number of Bodies (N)', fontsize=12, fontweight='bold')
    ax.set_ylabel('Average Execution Time per Tick (ms)', fontsize=12, fontweight='bold')
    
    ax.legend(title="Approximation Value", fontsize=11, title_fontsize=12)
    ax.grid(True, linestyle='--', alpha=0.7)

    plt.tight_layout()
    output_filename = 'Graph1_Scalability.png'
    plt.savefig(output_filename, dpi=300)
    print(f"Successfully generated {output_filename}")

if __name__ == "__main__":
    main()