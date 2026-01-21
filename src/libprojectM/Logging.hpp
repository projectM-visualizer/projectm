#pragma once

#include <projectM-4/projectM_cxx_export.h>

#include <cstdint>
#include <string>

namespace libprojectM {

/**
 * @class Logging
 * @brief A simple logger implementation to forward messages to the outside app.
 *
 * This class wraps logging functionality which can be used at either a global or thread-local
 * level, for both callbacks and log levels.
 */
class Logging
{
public:
    /**
     * The configurable log levels. If not set, "Information" is used by default.
     */
    enum class LogLevel : uint8_t
    {
        NotSet,      //!< Log level not set
        Trace,       //!< Most verbose logging, used to trace individual function calls or values.
        Debug,       //!< Debug-related messages for relevant data and developer information.
        Information, //!< Not-too-frequent messages possibly relevant for users and developers.
        Warning,     //!< Something is wrong, but doesn't affect execution in a major way.
        Error,       //!< A recoverable error occurred which negatively affects projectM, e.g. shader compilation issues.
        Fatal        //!< An unrecoverable error occurred and the projectM instance cannot continue execution.
    };

    /**
     * The application callback function.
     */
    using CallbackFunction = void (*)(const char* message, int severity, void* userData);

    /**
     * A struct holding the user callback function and data pointer.
     */
    struct UserCallback {
        CallbackFunction callbackFunction{}; //!< Function pointer of the user callback.
        void* userData{};                    //!< User data pointer for the callback.
    };

    Logging() = delete;

    /**
     * Sets the global callback function pointer used across all threads.
     * @param callback A UserCallback struct with the new function and user data pointers.
     */
    PROJECTM_CXX_EXPORT static void SetGlobalCallback(UserCallback callback);

    /**
     * Sets the thread-specific callback function pointer only used in the thread which registered it.
     * @param callback A UserCallback struct with the new function and user data pointers.
     */
    PROJECTM_CXX_EXPORT static void SetThreadCallback(UserCallback callback);

    /**
     * Sets the global log level used across all threads.
     * @param logLevel The log level to use. If set to LogLevel::NotSet, the value of m_defaultLogLevel is used.
     */
    PROJECTM_CXX_EXPORT static void SetGlobalLogLevel(LogLevel logLevel);

    /**
     * Sets the thread-specific log level only used in the thread which set it.
     * @param logLevel The log level to use. If set to LogLevel::NotSet, the value of m_defaultLogLevel is used.
     */
    PROJECTM_CXX_EXPORT static void SetThreadLogLevel(LogLevel logLevel);

    /**
     * Returns the effective log level for the current thread.
     * @return The log level set for this thread, or, if LogLevel::NotSet, the global log level.
     *         If no global log level is set, it returns the value of m_defaultLogLevel.
     */
    PROJECTM_CXX_EXPORT static auto GetLogLevel() -> LogLevel;

    /**
     * Returns whether a callback is registered or not.
     * @return true if a callback is registered for the current thread or globally, false if none is registered.
     */
    PROJECTM_CXX_EXPORT static auto HasCallback() -> bool;

    /**
     * @brief Passes a log message with the given severity to the active thread or global callback.
     * If no callbacks are registered, this function does nothing.
     * @param message
     * @param severity
     */
    PROJECTM_CXX_EXPORT static void Log(const std::string& message, LogLevel severity);

    /**
     * The default log level used if no log level is set (LogLevel::Information)
     */
    PROJECTM_CXX_EXPORT static const LogLevel m_defaultLogLevel;

private:
    /**
     * @brief Returns the active callback for this thread.
     * If the thread has a local callback, this is returned, otherwise the global callback.
     * @return A pointer to the active callback function, or nullptr if none is registered.
     */
    static auto GetLoggingCallback() -> UserCallback;

    static UserCallback m_globalCallback;              //!< The global callback function.
    thread_local static UserCallback m_threadCallback; //!< The thread-specific callback function.

    static LogLevel m_globalLogLevel;              //!< The global log level.
    thread_local static LogLevel m_threadLogLevel; //!< The thread-specific log level.
};

#ifdef ENABLE_DEBUG_LOGGING
#define LOG_TRACE(message)                                                            \
    if (Logging::HasCallback() && Logging::GetLogLevel() == Logging::LogLevel::Trace) \
    {                                                                                 \
        Logging::Log(message, Logging::LogLevel::Trace);                              \
    }

#define LOG_DEBUG(message)                                                            \
    if (Logging::HasCallback() && Logging::GetLogLevel() <= Logging::LogLevel::Debug) \
    {                                                                                 \
        Logging::Log(message, Logging::LogLevel::Debug);                              \
    }
#else
#define LOG_TRACE(message)
#define LOG_DEBUG(message)
#endif

#define LOG_INFO(message)                                                                   \
    if (Logging::HasCallback() && Logging::GetLogLevel() <= Logging::LogLevel::Information) \
    {                                                                                       \
        Logging::Log(message, Logging::LogLevel::Information);                              \
    }

#define LOG_WARN(message)                                                               \
    if (Logging::HasCallback() && Logging::GetLogLevel() <= Logging::LogLevel::Warning) \
    {                                                                                   \
        Logging::Log(message, Logging::LogLevel::Warning);                              \
    }

#define LOG_ERROR(message)                                                            \
    if (Logging::HasCallback() && Logging::GetLogLevel() <= Logging::LogLevel::Error) \
    {                                                                                 \
        Logging::Log(message, Logging::LogLevel::Error);                              \
    }

#define LOG_FATAL(message)                                                            \
    if (Logging::HasCallback() && Logging::GetLogLevel() <= Logging::LogLevel::Fatal) \
    {                                                                                 \
        Logging::Log(message, Logging::LogLevel::Fatal);                              \
    }

} // namespace libprojectM
