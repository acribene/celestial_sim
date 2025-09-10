#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H
#include <chrono>

using clocktype_t = std::chrono::high_resolution_clock;
using seconds_t = std::chrono::duration<double>;
using timepoint_t = std::chrono::time_point<clocktype_t>;

class TimeManager {
private:
  timepoint_t m_currentTime;
  double m_accumulator;
  double m_timeScale;
  double m_fixedDeltaTime;
  double m_maxFrameTime;

public:
  TimeManager(double fixedDeltaTime = 1.0/60);
  
  void update();
  double getFixedDeltaTime() const;
  double getTimeScale() const;
  void setTimeScale();
  void increaseTimeScale(float factor);
  void decreaseTimeScale(float factor);
  bool shouldUpdatePhysics() const;
  void consumePhysicsTime();
};
#endif // TIME_MANAGER_H
