#include "TimeKeeper.hpp"

#include "RandomNumberGenerators.hpp"

#include <algorithm>

TimeKeeper::TimeKeeper(double presetDuration, double smoothDuration, double hardcutDuration, double easterEgg)
    : m_easterEgg(easterEgg)
    , m_presetDuration(presetDuration)
    , m_softCutDuration(smoothDuration)
    , m_hardCutDuration(hardcutDuration)
{
    UpdateTimers();
}

void TimeKeeper::UpdateTimers()
{
    auto currentTime = std::chrono::high_resolution_clock::now();

    m_currentTime = std::chrono::duration<double>(currentTime - m_startTime).count();
    m_presetFrameA++;
    m_presetFrameB++;
}

void TimeKeeper::StartPreset()
{
    m_isSmoothing = false;
    m_presetTimeA = m_currentTime;
    m_presetFrameA = 1;
    m_presetDurationA = sampledPresetDuration();
}

void TimeKeeper::StartSmoothing()
{
    m_isSmoothing = true;
    m_presetTimeB = m_currentTime;
    m_presetFrameB = 1;
    m_presetDurationB = sampledPresetDuration();
}

void TimeKeeper::EndSmoothing()
{
    m_isSmoothing = false;
    m_presetTimeA = m_presetTimeB;
    m_presetFrameA = m_presetFrameB;
    m_presetDurationA = m_presetDurationB;
}

bool TimeKeeper::CanHardCut()
{
    return (m_currentTime - m_presetTimeA) > m_hardCutDuration;
}

double TimeKeeper::SmoothRatio()
{
    return (m_currentTime - m_presetTimeB) / m_softCutDuration;
}

bool TimeKeeper::IsSmoothing()
{
    return m_isSmoothing;
}

double TimeKeeper::GetRunningTime()
{
    return m_currentTime;
}

double TimeKeeper::PresetProgressA()
{
    if (m_isSmoothing)
    {
        return 1.0;
    }

    return (m_currentTime - m_presetTimeA) / m_presetDurationA;
}

double TimeKeeper::PresetProgressB()
{
    return (m_currentTime - m_presetTimeB) / m_presetDurationB;
}

int TimeKeeper::PresetFrameB()
{
    return m_presetFrameB;
}

int TimeKeeper::PresetFrameA()
{
    return m_presetFrameA;
}

int TimeKeeper::PresetTimeB()
{
    return m_presetTimeB;
}

int TimeKeeper::PresetTimeA()
{
    return m_presetTimeA;
}

double TimeKeeper::sampledPresetDuration()
{
    return std::max<double>(1, std::min<double>(60, RandomNumberGenerators::gaussian
        (m_presetDuration, m_easterEgg)));

}
