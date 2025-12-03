#include "Logging.hpp"

#include <string>

namespace libprojectM {

Logging::UserCallback Logging::m_globalCallback = {};
thread_local Logging::UserCallback Logging::m_threadCallback = {};

Logging::LogLevel Logging::m_globalLogLevel = LogLevel::NotSet;
thread_local Logging::LogLevel Logging::m_threadLogLevel = LogLevel::NotSet;

const Logging::LogLevel Logging::m_defaultLogLevel = LogLevel::Information;

void Logging::SetGlobalCallback(const UserCallback callback)
{
    m_globalCallback = callback;
}

void Logging::SetThreadCallback(const UserCallback callback)
{
    m_threadCallback = callback;
}

void Logging::SetGlobalLogLevel(const LogLevel logLevel)
{
    m_globalLogLevel = logLevel;
}

void Logging::SetThreadLogLevel(const LogLevel logLevel)
{
    m_threadLogLevel = logLevel;
}

auto Logging::GetLogLevel() -> LogLevel
{
    if (m_threadLogLevel != LogLevel::NotSet)
    {
        return m_threadLogLevel;
    }

    if (m_globalLogLevel != LogLevel::NotSet)
    {
        return m_globalLogLevel;
    }

    return m_defaultLogLevel;
}

auto Logging::HasCallback() -> bool
{
    return GetLoggingCallback().callbackFunction != nullptr;
}

void Logging::Log(const std::string& message, LogLevel severity)
{
    auto callback = GetLoggingCallback();

    if (callback.callbackFunction == nullptr)
    {
        return;
    }

    callback.callbackFunction(message.c_str(), static_cast<int>(severity), callback.userData);
}

auto Logging::GetLoggingCallback() -> UserCallback
{
    if (m_threadCallback.callbackFunction != nullptr)
    {
        return m_threadCallback;
    }

    return m_globalCallback;
}

} // namespace libprojectM
