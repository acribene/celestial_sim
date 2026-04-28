#include "../headers/benchmark.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include "../headers/simulation.h"

void benchmark::runHeadlessBenchmark(int numBodies, double theta, int totalTicks, years_t fixedDeltaT, std::ofstream& csv) {
    std::cout << "[BENCHMARK] Testing N=" << numBodies << " | Theta=" << theta << "...\n";

    Simulation sim(theta);
    sim.loadPreset(2, numBodies); // 2 = Random Stable Disk
    
    double initialEnergy = sim.calculateTotalEnergy();

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < totalTicks; i++) {
        sim.update(fixedDeltaT, true);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    double avgTimeMs = static_cast<double>(duration.count()) / totalTicks;
    double finalEnergy = sim.calculateTotalEnergy();

    csv << numBodies << "," 
        << theta << "," 
        << avgTimeMs << "," 
        << initialEnergy << "," 
        << finalEnergy << "\n";
}

void benchmark::runAllBenchmarks() {
    std::cout << "=== STARTING SCALABILITY BENCHMARKS ===\n";
    
    std::ofstream csv("scalability_curve_results_WITHCOLLISIONS.csv");
    if (!csv.is_open()) {
        std::cerr << "Failed to open scalability_curve_results.csv for writing!\n";
        return;
    }

    csv << "N,Theta,AvgTimeMs,InitialTotalEnergy,FinalTotalEnergy\n";

    // Standardize the test conditions
    int ticksToRun = 500;
    
    // Construct the years_t duration using your constant
    years_t fixedDeltaT(TIME_STEP); 

    // The body counts we want to test to show the curve
    std::vector<int> testBodyCounts = {100, 500, 1000, 2500, 5000, 10000};
    
    // The thetas we want to test: 0.0 (O(N^2) baseline) to 1.5 barnes hut)
    std::vector<double> testThetas = {0.0, 0.5, 1.0, 1.5};

    // Run the nested loops for the Scalability Curve
    for (double theta : testThetas) {
        std::cout << "\n--- Running series for Theta = " << theta << " ---\n";
        for (int n : testBodyCounts) {
            if (theta == 0.0 && n > 5000) {
                std::cout << "[BENCHMARK] Skipping N=" << n << " for Theta=" << theta << " to save time.\n";
                continue;
            }
            runHeadlessBenchmark(n, theta, ticksToRun, fixedDeltaT, csv);
        }
    }

    csv.close();
    std::cout << "\n=== BENCHMARKS COMPLETE ===\n";
    std::cout << "Results saved to scalability_curve_results.csv\n";
}