#include "TimeManager.h"
#include <algorithm>

TimeManager::TimeManager(double fixedDeltaTime) : m_currentTime(clocktype_t::now()), m_accumulator(0), m_timeScale(1),
    m_fixedDeltaTime(fixedDeltaTime), m_maxFrameTime(0.1)
{}
