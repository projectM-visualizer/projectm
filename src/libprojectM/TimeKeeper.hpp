#pragma once

#include <chrono>
#include <random>

namespace libprojectM {

class TimeKeeper
{

public:
    TimeKeeper(double presetDuration, double smoothDuration, double hardcutDuration, double easterEgg);


    void SetFrameTime(double secondsSinceStart);


    double GetFrameTime() const;


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

    double PresetTimeA();

    double PresetTimeB();

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

    inline auto SecondsSinceLastFrame() const -> double
    {
        return m_secondsSinceLastFrame;
    }

private:

    std::chrono::high_resolution_clock::time_point m_startTime{std::chrono::high_resolution_clock::now()};

    std::random_device m_randomDevice{};
    std::mt19937 m_randomGenerator{m_randomDevice()};

    double m_userSpecifiedTime{-1.0};

    double m_secondsSinceLastFrame{};

    double m_easterEgg{};

    double m_presetDuration{};
    double m_presetDurationA{};
    double m_presetDurationB{};
    double m_softCutDuration{};
    double m_hardCutDuration{};

    double m_currentTime{};
    double m_presetTimeA{};
    double m_presetTimeB{};

    int m_presetFrameA{};
    int m_presetFrameB{};

    bool m_isSmoothing{false};
};

}
