#ifndef BENCHMARK_H
#define BENCHMARK

#include <fstream>
#include "../utils/constants.h"

namespace benchmark
{
    void runHeadlessBenchmark(int numBodies, double theta, int totalTicks, years_t fixedDeltaT, std::ofstream& csv) ;
    void runAllBenchmarks();
}

#endif // BENCHMARK_H