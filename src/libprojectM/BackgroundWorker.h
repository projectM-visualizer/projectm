#pragma once

#include <condition_variable>
#include <mutex>

/**
 * Small class to encapsulate synchronization of a simple background task runner
 */
class BackgroundWorkerSync
{
public:
    BackgroundWorkerSync() = default;

    void Reset()
    {
        m_isWorkToDo = false;
        m_finished = false;
    }

    // called by foreground
    void WakeUpBackgroundTask()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_isWorkToDo = true;
        m_conditionStartWork.notify_one();
    }

    // called by foreground
    void WaitForBackgroundTaskToFinish()
    {
        std::unique_lock<std::mutex> guard(m_mutex);
        while (m_isWorkToDo)
        {
            m_conditionWorkDone.wait(guard);
        }
    }

    // called by foreground() when shutting down, background thread should exit
    void FinishUp()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_finished = true;
        m_conditionStartWork.notify_all();
    }

    // called by background
    auto WaitForWork() -> bool
    {
        std::unique_lock<std::mutex> guard(m_mutex);
        while (!m_isWorkToDo && !m_finished)
        {
            m_conditionStartWork.wait(guard);
        }
        return !m_finished;
    }

    // called by background
    void FinishedWork()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_isWorkToDo = false;
        m_conditionWorkDone.notify_one();
    }

private:
    mutable std::mutex m_mutex; //!< Mutex that controls access to the work flags.
    std::condition_variable m_conditionStartWork;
    std::condition_variable m_conditionWorkDone;
    volatile bool m_isWorkToDo{ false };
    volatile bool m_finished{ false };

};
