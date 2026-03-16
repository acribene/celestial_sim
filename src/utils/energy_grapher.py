import pandas as pd
import matplotlib.pyplot as plt
import os
import sys
import argparse

# 1. Setup command-line argument parsing for a SINGLE file
parser = argparse.ArgumentParser(description="Graph total energy for a single simulation log.")
parser.add_argument('file', help="Path to the CSV file you want to graph")

try:
    args = parser.parse_args()
except SystemExit:
    print("\nUsage example: python energy_grapher.py energy_log_preset_500yearEARTHRK4.csv")
    sys.exit(1)

filepath = args.file

if not os.path.exists(filepath):
    print(f"Error: Could not find file '{filepath}'.")
    sys.exit(1)

# Extract a clean name for the title
filename_only = os.path.splitext(os.path.basename(filepath))[0]
display_name = filename_only.replace('_', ' ').title()

# 2. Load and process the data
print(f"Loading data from: {filepath}")
data = pd.read_csv(filepath)

# --- PHYSICS CONVERSION FACTORS ---
M_sun = 1.98847e30     # 1 Solar Mass in kg
AU = 1.495978707e11    # 1 Astronomical Unit in meters
yr = 31557600          # 1 Earth Year in seconds (365.25 days)

# Calculate exactly how many Joules are in 1 Simulation Energy Unit
joules_per_unit = M_sun * (AU**2) / (yr**2)

# --- CREATE THE PLOT ---
fig, ax1 = plt.subplots(figsize=(11, 7))

# Plot the primary data on the left Y-axis (Total Energy)
color_left = '#2980b9'
ax1.plot(data['Time'], data['TotalEnergy'], label='Total Energy', color=color_left, linewidth=2.5)

# Left Axis Formatting (Simulation Units)
ax1.set_xlabel('Simulation Time (Earth Years)', fontsize=12, fontweight='bold')
ax1.set_ylabel('Energy ($M_{\\odot} \\cdot AU^2 / yr^2$)', fontsize=12, fontweight='bold', color=color_left)
ax1.tick_params(axis='y', labelcolor=color_left)
ax1.ticklabel_format(useOffset=False, style='plain', axis='y')
ax1.grid(True, linestyle='--', alpha=0.7)

# --- ADD SECONDARY Y-AXIS (JOULES) ---
ax2 = ax1.twinx()  

# Dynamically sync the right axis limits to the left axis's auto-scaled limits
y1_min, y1_max = ax1.get_ylim()
ax2.set_ylim(y1_min * joules_per_unit, y1_max * joules_per_unit)

color_right = '#e74c3c'
ax2.set_ylabel('Energy (Joules)', fontsize=12, fontweight='bold', color=color_right)
ax2.tick_params(axis='y', labelcolor=color_right)
ax2.ticklabel_format(useOffset=False, style='plain', axis='y')

# --- TITLES AND ANNOTATIONS ---
plt.title(f'Total System Energy Over Time\n[{display_name}]', 
          fontsize=14, fontweight='bold')

# Add a clean text box explaining the conversion
textstr = f"1 Simulation Unit = {joules_per_unit:.2e} Joules"
props = dict(boxstyle='round', facecolor='white', alpha=0.9, edgecolor='gray')
ax1.text(0.02, 0.04, textstr, transform=ax1.transAxes, fontsize=11,
         verticalalignment='bottom', bbox=props)

plt.tight_layout()
plt.show()