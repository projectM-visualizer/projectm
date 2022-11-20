#pragma once

#include <chrono>

class TimeKeeper
{

public:

    TimeKeeper(double presetDuration, double smoothDuration, double hardcutDuration, double easterEgg);

    void UpdateTimers();

    void StartPreset();

    void StartSmoothing();

    void EndSmoothing();

    bool CanHardCut();

    double SmoothRatio();

    bool IsSmoothing();

    double GetRunningTime();

    double PresetProgressA();

    double PresetProgressB();

    int PresetFrameA();

    int PresetFrameB();

    int PresetTimeA();

    int PresetTimeB();

    double sampledPresetDuration();

    inline void ChangeHardCutDuration(int seconds)
    {
        m_hardCutDuration = seconds;
    }

    inline void ChangeHardCutDuration(double seconds)
    {
        m_hardCutDuration = seconds;
    }

    inline void ChangeSoftCutDuration(int seconds)
    {
        m_softCutDuration = seconds;
    }

    inline void ChangeSoftCutDuration(double seconds)
    {
        m_softCutDuration = seconds;
    }

    inline void ChangePresetDuration(int seconds)
    {
        m_presetDuration = seconds;
    }

    inline auto PresetDuration() const -> double
    {
        return m_presetDuration;
    }

    inline void ChangePresetDuration(double seconds)
    {
        m_presetDuration = seconds;
    }

    inline void ChangeEasterEgg(float value)
    {
        m_easterEgg = value;
    }

private:

    /* The first ticks value of the application */
    std::chrono::high_resolution_clock::time_point m_startTime{ std::chrono::high_resolution_clock::now() };

    double m_easterEgg{ 0.0 };

    double m_presetDuration{ 0.0 };
    double m_presetDurationA{ 0.0 };
    double m_presetDurationB{ 0.0 };
    double m_softCutDuration{ 0.0 };
    double m_hardCutDuration{ 0.0 };

    double m_currentTime{ 0.0 };
    double m_presetTimeA{ 0.0 };
    double m_presetTimeB{ 0.0 };

    int m_presetFrameA{ 0 };
    int m_presetFrameB{ 0 };

    bool m_isSmoothing{ false };


};
