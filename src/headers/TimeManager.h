#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H
#include <chrono>
#include "../utils/constants.h"

class TimeManager {
private:
  timepoint_t m_currentTime; // The current time
  years_t m_accumulator; // Accumulates actual time passed between frames
  double m_timeScale; // The rate at which simulation is running, ie.. 1.0 == real time
  years_t m_fixedDeltaTime; // The rate at which we want our physics updating
  seconds_t m_maxFrameTime; // Used to clamp our measured frame time in the event of lag

public:
  TimeManager(years_t fixedStep = years_t(1.0 / 365.25 / 60.0));
  
  void update();
  seconds_t getFixedDeltaTime() const;
  double getTimeScale() const;
  void setTimeScale(double scale);
  void increaseTimeScale(float factor);
  void decreaseTimeScale(float factor);
  bool shouldUpdatePhysics() const;
  void consumePhysicsTime();
};
#endif // TIME_MANAGER_H
