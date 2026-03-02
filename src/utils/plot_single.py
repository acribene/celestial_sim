import pandas as pd
import matplotlib.pyplot as plt
import argparse
import os
import sys

# 1. Setup command-line argument parsing for a SINGLE file
parser = argparse.ArgumentParser(description="Graph fractional energy drift for a single simulation log.")
parser.add_argument('file', help="Path to the CSV file you want to graph")

try:
    args = parser.parse_args()
except SystemExit:
    print("\nUsage example: python plot_single.py energy_log_rk4.csv")
    sys.exit(1)

filepath = args.file

if not os.path.exists(filepath):
    print(f"Error: Could not find file '{filepath}'.")
    sys.exit(1)

# Extract a clean name for the title
filename_only = os.path.splitext(os.path.basename(filepath))[0]
display_name = filename_only.replace('_', ' ').title()

# 2. Load and process the data
df = pd.read_csv(filepath)

# Calculate Fractional Energy Error: (E(t) - E(0)) / |E(0)|
e0 = df['TotalEnergy'].iloc[0]
df['EnergyError'] = (df['TotalEnergy'] - e0) / abs(e0)

# 3. Create the plot
plt.figure(figsize=(10, 6))

# Plot the data
plt.plot(df['Time'], df['EnergyError'], label=display_name, color='#2980b9', linewidth=2)

# --- PERFECT ZOOM LOGIC ---
# Find the exact min and max of this specific dataset
min_err = df['EnergyError'].min()
max_err = df['EnergyError'].max()

# Calculate a 10% padding margin so the line doesn't touch the top/bottom of the graph
data_range = max_err - min_err
if data_range == 0:
    margin = 1e-12 # Fallback if the line is mathematically 100% perfectly flat
else:
    margin = data_range * 0.10

# Apply the precise zoom limits
plt.ylim(min_err - margin, max_err + margin)
# --------------------------

# Formatting
plt.title(f'Fractional Energy Error Over Time\n({display_name})', fontsize=14, fontweight='bold')
plt.xlabel('Simulation Time (Earth Years)', fontsize=12, fontweight='bold')
plt.ylabel('Fractional Energy Error ($\Delta E / E_0$)', fontsize=12, fontweight='bold')

# Use scientific notation for the Y-axis to easily read tiny numbers
plt.ticklabel_format(axis='y', style='sci', scilimits=(0,0))
plt.grid(True, linestyle='--', alpha=0.7)

# Draw a subtle reference line at exactly 0 (no error)
plt.axhline(0, color='black', linewidth=1.5, alpha=0.5) 

plt.legend(fontsize=12, loc='best')
plt.tight_layout()

# Save the output dynamically based on the input name
output_img = f"plot_{filename_only}.png"
plt.savefig(output_img, dpi=300)
print(f"Saved perfectly zoomed graph to {output_img}")

plt.show()