#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>
#include <chrono>

// Header file full of constants for whatever reasons we may need.
const constexpr double DAYS_PER_YEAR = 365.25;
const double HOURS_PER_DAY = 24.0;
const double MINUTES_PER_HOUR = 60.0;
const double SECONDS_PER_MINUTE = 60.0;

const constexpr double GC = (4.0 * M_PI * M_PI); // Gravitational constant
const double SOFTENING = 1e-10; // Softening parameter to avoid division by zero
const constexpr double TIME_STEP = (1.0 / (365.25 * 100.0)); // 0.01 days in years for stability

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const double SCALE = 100.0; // pixels per AU

// types for time
using clocktype_t = std::chrono::high_resolution_clock;
using seconds_t = std::chrono::duration<double>;
using years_t = std::chrono::duration<double,  std::ratio<31557600>>;
using timepoint_t = std::chrono::time_point<clocktype_t>;

#endif // CONSTANTS_H
