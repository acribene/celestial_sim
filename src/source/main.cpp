#include "raylib.h"
#include "../headers/Application.h"
#include "../headers/benchmark.h"

int main() {

    bool run_benchmarks = true;
    if(run_benchmarks)
    {
        benchmark::runAllBenchmarks();
        return 0;
    }

    Application sim(WINDOW_WIDTH, WINDOW_HEIGHT);
    sim.run();

    return 0;
}
