import pandas as pd
import matplotlib.pyplot as plt
import argparse
import os
import sys

parser = argparse.ArgumentParser(description="Graph thermodynamic energy conservation.")
parser.add_argument('file', help="Path to the CSV file with HeatEnergy logging")

try:
    args = parser.parse_args()
except SystemExit:
    print("\nUsage example: python plot_thermodynamics.py energy_log_preset_1.csv")
    sys.exit(1)

filepath = args.file
if not os.path.exists(filepath):
    print(f"Error: Could not find file '{filepath}'.")
    sys.exit(1)

# Load the data
df = pd.read_csv(filepath)

# Ensure the new column exists
if 'HeatEnergy' not in df.columns:
    print("Error: 'HeatEnergy' column not found! Did you recompile your C++ code?")
    sys.exit(1)

# Base mechanical energy at Time = 0
e0_mech = df['TotalEnergy'].iloc[0]

# Calculate the fractional fractions
# 1. Mechanical Energy Error: (E_mech(t) - E_mech(0)) / |E_mech(0)|
df['Mech_Error'] = (df['TotalEnergy'] - e0_mech) / abs(e0_mech)

# 2. Heat Energy Fractional: Heat(t) / |E_mech(0)|
df['Heat_Fraction'] = df['HeatEnergy'] / abs(e0_mech)

# 3. True Total Energy Error: (E_mech(t) + Heat(t) - E_mech(0)) / |E_mech(0)|
df['True_Total_Error'] = ((df['TotalEnergy'] + df['HeatEnergy']) - e0_mech) / abs(e0_mech)

# Create the plot
plt.figure(figsize=(11, 7))

# Plot the three lines
plt.plot(df['Time'], df['Mech_Error'], label='Mechanical Energy (Orbits)', color='#3498db', linewidth=2, linestyle='--')
plt.plot(df['Time'], df['Heat_Fraction'], label='Thermal Energy (Collisions/Friction)', color='#e74c3c', linewidth=2, linestyle='--')
plt.plot(df['Time'], df['True_Total_Error'], label='True Total Energy (Mech + Thermal)', color='#2ecc71', linewidth=3)

# Find absolute max for perfect symmetric zooming
max_err = max(df['Mech_Error'].abs().max(), df['Heat_Fraction'].abs().max())
margin = max(max_err * 0.10, 1e-10) # 10% margin, with a tiny fallback if everything is 0

plt.ylim(-max_err - margin, max_err + margin)

# Formatting
plt.title('Thermodynamic Energy Conservation\n(First Law of Thermodynamics)', fontsize=15, fontweight='bold')
plt.xlabel('Simulation Time (Earth Years)', fontsize=12, fontweight='bold')
plt.ylabel('Fractional Energy Change ($\Delta E / |E_0|$)', fontsize=12, fontweight='bold')

plt.ticklabel_format(axis='y', style='sci', scilimits=(0,0))
plt.grid(True, linestyle=':', alpha=0.7)
plt.axhline(0, color='black', linewidth=1, alpha=0.5)

plt.legend(fontsize=12, loc='best', framealpha=0.9)
plt.tight_layout()

filename_only = os.path.splitext(os.path.basename(filepath))[0]
output_img = f"thermodynamics_{filename_only}.png"
plt.savefig(output_img, dpi=300)
print(f"Saved thermodynamic proof to {output_img}")

plt.show()