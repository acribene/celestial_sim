import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

# Find all preset log files
list_of_files = glob.glob('energy_log_preset_*.csv')

if not list_of_files:
    print("Error: No energy log files found. Run a simulation preset first!")
    exit()

# Get the most recently modified file
latest_file = max(list_of_files, key=os.path.getmtime)
print(f"Loading data from: {latest_file}")

data = pd.read_csv(latest_file)

# --- PHYSICS CONVERSION FACTORS ---
M_sun = 1.98847e30     # 1 Solar Mass in kg
AU = 1.495978707e11    # 1 Astronomical Unit in meters
yr = 31557600          # 1 Earth Year in seconds (365.25 days)

# Calculate exactly how many Joules are in 1 Simulation Energy Unit
joules_per_unit = M_sun * (AU**2) / (yr**2)

# --- CREATE THE PLOT ---
fig, ax1 = plt.subplots(figsize=(11, 7))

# Plot the primary data on the left Y-axis
color_left = '#2980b9'
ax1.plot(data['Time'], data['TotalEnergy'], label='Total Energy', color=color_left, linewidth=2.5)

# Calculate a 5% margin to prevent Matplotlib from zooming into floating-point noise
energy_mean = data['TotalEnergy'].mean()
margin = abs(energy_mean * 0.05) 
ax1.set_ylim(energy_mean - margin, energy_mean + margin)

# Left Axis Formatting (Simulation Units)
ax1.set_xlabel('Simulation Time (Earth Years)', fontsize=12, fontweight='bold')
ax1.set_ylabel('Energy ($M_{\odot} \cdot AU^2 / yr^2$)', fontsize=12, fontweight='bold', color=color_left)
ax1.tick_params(axis='y', labelcolor=color_left)
ax1.ticklabel_format(useOffset=False, style='plain', axis='y')
ax1.grid(True, linestyle='--', alpha=0.7)

# --- ADD SECONDARY Y-AXIS (JOULES) ---
ax2 = ax1.twinx()  
ax2.set_ylim((energy_mean - margin) * joules_per_unit, (energy_mean + margin) * joules_per_unit)
color_right = '#e74c3c'
ax2.set_ylabel('Energy (Joules)', fontsize=12, fontweight='bold', color=color_right)
ax2.tick_params(axis='y', labelcolor=color_right)

# --- TITLES AND ANNOTATIONS ---
plt.title('Total System Energy Over Time', 
          fontsize=14, fontweight='bold')

# Add a clean text box explaining the conversion
textstr = f"1 Simulation Unit = {joules_per_unit:.2e} Joules"
props = dict(boxstyle='round,pad=0.5', facecolor='white', alpha=0.9, edgecolor='gray')
ax1.text(0.02, 0.95, textstr, transform=ax1.transAxes, fontsize=11,
        verticalalignment='top', bbox=props)

fig.tight_layout()

# Save the graph automatically as an image for your presentation
plt.savefig("presentation_graph.png", dpi=300)
print("Saved highly-detailed graph to presentation_graph.png")

# Show the interactive plot
plt.show()