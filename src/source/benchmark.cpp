#include "../headers/benchmark.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include "../headers/simulation.h"

void benchmark::runHeadlessBenchmark(int numBodies, double theta, int totalTicks, years_t fixedDeltaT, std::ofstream& csv) {
    std::cout << "[BENCHMARK] Testing N=" << numBodies << " | Theta=" << theta << "...\n";

    Simulation sim(theta);
    
    // Load the pre-generated master state specific to this N
    std::string filename = "master_benchmark_N_" + std::to_string(numBodies) + ".sim";
    sim.loadSimulation(filename); 
    
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
    
    int ticksToRun = 500;
    years_t fixedDeltaT(TIME_STEP); 

    std::vector<int> testBodyCounts = {100, 500, 1000, 2500, 5000, 10000};
    
    std::vector<double> testThetas = {0.0, 0.5, 1.0, 1.5}; 

    std::cout << "\n--- Phase 1: Generating Master States ---\n";
    for (int n : testBodyCounts) {
        std::string filename = "master_benchmark_N_" + std::to_string(n) + ".sim";
        std::cout << "Generating " << filename << "...\n";
        
        Simulation masterSim(0.0); 
        masterSim.loadPreset(2, n);
        masterSim.saveSimulation(filename);
    }

    // --- PHASE 2: RUN BENCHMARKS ---
    std::cout << "\n--- Phase 2: Executing Benchmark Loops ---\n";
    
    std::ofstream csv("scalability_curve_results_WITHCOLLISIONS.csv");
    if (!csv.is_open()) {
        std::cerr << "Failed to open CSV for writing!\n";
        return;
    }

    csv << "N,Theta,AvgTimeMs,InitialTotalEnergy,FinalTotalEnergy\n";

    for (double theta : testThetas) {
        std::cout << "\n--- Running series for Theta = " << theta << " ---\n";
        for (int n : testBodyCounts) {
            
            // Safety valve to prevent O(N^2) lockups on massive clusters
            if (theta == 0.0 && n > 5000) {
                std::cout << "[BENCHMARK] Skipping N=" << n << " for Theta=0.0 to save time.\n";
                continue;
            }
            runHeadlessBenchmark(n, theta, ticksToRun, fixedDeltaT, csv);
        }
    }

    csv.close();
    std::cout << "\n=== BENCHMARKS COMPLETE ===\n";
}