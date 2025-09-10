#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H
#include <chrono>

using clocktype_t = std::chrono::high_resolution_clock;
using seconds_t = std::chrono::duration<double>;
using timepoint_t = std::chrono::time_point<clocktype_t>;

class TimeManager {
private:
  timepoint_t m_currentTime; // The current time
  seconds_t m_accumulator; // Accumulates actual time passed between frames
  double m_timeScale; // The rate at which simulation is running, ie.. 1.0 == real time
  seconds_t m_fixedDeltaTime; // The rate at which we want our physics updating
  seconds_t m_maxFrameTime; // Used to clamp our measured frame time in the event of lag

public:
  TimeManager(double fixedDeltaTime = 1.0/60);
  
  void update();
  double getFixedDeltaTime() const;
  double getTimeScale() const;
  void setTimeScale(double scale);
  void increaseTimeScale(float factor);
  void decreaseTimeScale(float factor);
  bool shouldUpdatePhysics() const;
  void consumePhysicsTime();
};
#endif // TIME_MANAGER_H
