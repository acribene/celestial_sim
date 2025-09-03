#ifndef CONSTANTS_H
define CONSTANTS_H

#include <cmath>

// Header file full of constants for whatever reasons we may need.
const constexpr double DAYS_PER_YEAR = 365.25;
const double HOURS_PER_DAY = 24.0;
const double MINUTES_PER_HOUR = 60.0;
const double SECONDS_PER_MINUTE = 60.0;

const constexpr double GC = (4.0 * PI * PI); // Gravitational constant
const double DT = 1.0 / DAYS_PER_YEAR;

// RAYLIB CONSTANTS
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const constexpr double TIME_STEP = 1.0 / 365.25; // 1 day in years
const double SCALE = 100.0; // pixels per AU

#endif // CONSTANTS_H
