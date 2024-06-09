#pragma once

#include <chrono>
#include <random>

namespace libprojectM {

class TimeKeeper
{

public:
    TimeKeeper(double presetDuration, double smoothDuration, double hardcutDuration, double easterEgg);

    /**
     * @brief Sets a custom time value to use instead of the system time.
     * If less than zero, the system time will be used instead.
     * @param secondsSinceStart Fractional seconds since rendering the first frame.
     */
    void SetFrameTime(double secondsSinceStart);

    /**
     * @brief Gets the time of the last frame rendered.
     * @note This will not return the value set with SetFrameTime, but the actual time used to render the last frame.
     *       If a user-specified frame time was set, this value is returned. Otherwise, the frame time measured via the
     *       system clock will be returned.
     * @return Seconds elapsed rendering the last frame since starting projectM.
     */
    double GetFrameTime() const;

    /**
     * @brief Updates internal timers with either the system clock or a user-specified time value.
     */
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
    /* The first ticks value of the application */
    std::chrono::high_resolution_clock::time_point m_startTime{std::chrono::high_resolution_clock::now()};

    std::random_device m_randomDevice{};
    std::mt19937 m_randomGenerator{m_randomDevice()};

    double m_userSpecifiedTime{-1.0}; //!< User-specifed run time. If set to a value >= 0.0, this time is used instead of the system clock.

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

} // namespace libprojectM
