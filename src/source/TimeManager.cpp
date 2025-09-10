#include "../headers/TimeManager.h"
#include <algorithm>

TimeManager::TimeManager(years_t fixedDeltaTime) : m_currentTime(clocktype_t::now()), m_accumulator(0), m_timeScale(1),
    m_fixedDeltaTime(fixedDeltaTime), m_maxFrameTime(0.1)
{}

// Updates the time elapsed and sets up framtime for physics calculations
void TimeManager::update()
{
    auto new_time = clocktype_t::now();
    seconds_t frameTime = new_time - m_currentTime;
    m_currentTime = new_time;

    // Clamping the frame time so lags don't spiral
    frameTime = std::min(frameTime, m_maxFrameTime);
    m_accumulator += std::chrono::duration_cast<years_t>(frameTime) * m_timeScale;
}

seconds_t TimeManager::getFixedDeltaTime() const
{
    return m_fixedDeltaTime;
}

double TimeManager::getTimeScale() const
{
    return m_timeScale;
}

void TimeManager::setTimeScale(double scale)
{
    m_timeScale = std::max(0.0, scale);
}

void TimeManager::increaseTimeScale(float factor)
{
    setTimeScale(m_timeScale * factor);
}

void TimeManager::decreaseTimeScale(float factor)
{
    setTimeScale(m_timeScale / factor);
}

bool TimeManager::shouldUpdatePhysics() const
{
    return m_accumulator >= m_fixedDeltaTime;
}

// Subtracts our delta time from our accumulator
void TimeManager::consumePhysicsTime()
{
    m_accumulator -= m_fixedDeltaTime;
}
