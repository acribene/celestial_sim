#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

class TimeManager {
private:
  double m_currentTime;
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
