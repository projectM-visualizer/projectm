#include <atomic>
#include <gtest/gtest.h>

#include <Logging.hpp>

#include <atomic>
#include <thread>

using libprojectM::Logging;

class LoggingTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Logging::SetThreadCallback({});
        Logging::SetGlobalCallback({});
        Logging::SetThreadLogLevel(Logging::LogLevel::NotSet);
        Logging::SetGlobalLogLevel(Logging::LogLevel::NotSet);

        m_logFlag1 = false;
        m_logFlag2 = false;
    }

    static void loggingCallback1(const char*, int, void*)
    {
        m_logFlag1 = true;
    }

    static void loggingCallback2(const char*, int, void*)
    {
        m_logFlag2 = true;
    }

    static std::atomic_bool m_logFlag1;
    static std::atomic_bool m_logFlag2;

    static std::atomic_int m_logSeverity1;
    static std::atomic_int m_logSeverity2;
};

std::atomic_bool LoggingTest::m_logFlag1{};
std::atomic_bool LoggingTest::m_logFlag2{};

std::atomic_int LoggingTest::m_logSeverity1{};
std::atomic_int LoggingTest::m_logSeverity2{};

class LoggingTestWithLevels : public LoggingTest, public ::testing::WithParamInterface<Logging::LogLevel>
{
};

TEST_F(LoggingTest, GlobalCallback)
{
    EXPECT_FALSE(m_logFlag1);

    Logging::Log("Test", Logging::LogLevel::Information);
    EXPECT_FALSE(m_logFlag1);

    EXPECT_FALSE(Logging::HasCallback());

    Logging::SetGlobalCallback({&LoggingTest::loggingCallback1, nullptr});

    EXPECT_TRUE(Logging::HasCallback());

    Logging::Log("Test", Logging::LogLevel::Information);
    EXPECT_TRUE(m_logFlag1);
}

TEST_F(LoggingTest, ThreadCallback)
{
    EXPECT_FALSE(m_logFlag1);

    Logging::Log("Test", Logging::LogLevel::Information);
    EXPECT_FALSE(m_logFlag1);

    EXPECT_FALSE(Logging::HasCallback());

    Logging::SetThreadCallback({&LoggingTest::loggingCallback1, nullptr});

    EXPECT_TRUE(Logging::HasCallback());

    Logging::Log("Test", Logging::LogLevel::Information);
    EXPECT_TRUE(m_logFlag1);
}

TEST_F(LoggingTest, ThreadCallbackOtherThread)
{
    EXPECT_FALSE(m_logFlag1);
    EXPECT_FALSE(m_logFlag2);

    Logging::Log("Test", Logging::LogLevel::Information);

    EXPECT_FALSE(m_logFlag1);
    EXPECT_FALSE(m_logFlag2);

    Logging::SetGlobalCallback({&LoggingTest::loggingCallback1, nullptr});

    // Register thread-local callback, should override global callback
    {
        auto loggingThreadFunc = [&]() -> void {
            Logging::SetThreadCallback({&LoggingTest::loggingCallback2, nullptr});
            Logging::Log("Test", Logging::LogLevel::Information);
        };

        std::thread loggingThread{loggingThreadFunc};
        loggingThread.join();
    }

    EXPECT_FALSE(m_logFlag1);
    EXPECT_TRUE(m_logFlag2);
}

TEST_F(LoggingTest, GlobalCallbackOtherThread)
{
    EXPECT_FALSE(m_logFlag1);
    EXPECT_FALSE(m_logFlag2);

    Logging::Log("Test", Logging::LogLevel::Information);

    EXPECT_FALSE(m_logFlag1);
    EXPECT_FALSE(m_logFlag2);

    Logging::SetGlobalCallback({&LoggingTest::loggingCallback1, nullptr});

    // No thread-local callback, should use global callback
    {
        auto loggingGlobalFunc = [&]() -> void {
            Logging::Log("Test", Logging::LogLevel::Information);
        };

        std::thread loggingThread{loggingGlobalFunc};
        loggingThread.join();
    }

    EXPECT_TRUE(m_logFlag1);
    EXPECT_FALSE(m_logFlag2);
}

TEST_F(LoggingTest, GlobalCallbackUserData)
{
    static void* returnedPointer{};

    const Logging::CallbackFunction logCallback = [](const char*, int, void* userData) -> void {
        returnedPointer = userData;
    };

    Logging::Log("Test", Logging::LogLevel::Information);
    EXPECT_FALSE(m_logFlag1);

    EXPECT_FALSE(Logging::HasCallback());

    // We use the lambda address to "randomize" the data.
    auto* testPointer{reinterpret_cast<void*>(logCallback)};
    Logging::SetGlobalCallback({logCallback, testPointer});

    EXPECT_TRUE(Logging::HasCallback());

    Logging::Log("Test", Logging::LogLevel::Information);
    EXPECT_EQ(returnedPointer, testPointer);
}

TEST_F(LoggingTest, ThreadCallbackUserData)
{
    static void* returnedPointer{};

    const Logging::CallbackFunction logCallback = [](const char*, int, void* userData) -> void {
        returnedPointer = userData;
    };

    Logging::Log("Test", Logging::LogLevel::Information);
    EXPECT_FALSE(m_logFlag1);

    EXPECT_FALSE(Logging::HasCallback());

    // We use the lambda address to "randomize" the data.
    auto* testPointer{reinterpret_cast<void*>(logCallback)};
    Logging::SetThreadCallback({logCallback, testPointer});

    EXPECT_TRUE(Logging::HasCallback());

    Logging::Log("Test", Logging::LogLevel::Information);
    EXPECT_EQ(returnedPointer, testPointer);
}

TEST_F(LoggingTest, MessageInGlobalCallback)
{
    static std::string logMessage;

    const Logging::CallbackFunction logCallback = [](const char* message, int, void*) -> void {
        logMessage = message;
    };

    Logging::SetGlobalCallback({logCallback, nullptr});
    logMessage = {};

    Logging::Log("This is a test message", Logging::LogLevel::Information);
    ASSERT_EQ(logMessage, "This is a test message");
}

TEST_F(LoggingTest, MessageInThreadCallback)
{
    static std::string logMessage;

    const Logging::CallbackFunction logCallback = [](const char* message, int, void*) -> void {
        logMessage = message;
    };

    Logging::SetThreadCallback({logCallback, nullptr});
    logMessage = {};

    Logging::Log("This is a test message", Logging::LogLevel::Information);
    ASSERT_EQ(logMessage, "This is a test message");
}

TEST_P(LoggingTestWithLevels, SeverityInGlobalCallback)
{
    static Logging::LogLevel logLevel;

    Logging::CallbackFunction logCallback = [](const char*, int level, void*) -> void {
        logLevel = static_cast<Logging::LogLevel>(level);
    };

    Logging::SetGlobalCallback({logCallback, nullptr});
    logLevel = Logging::LogLevel::NotSet;

    Logging::Log("Test", GetParam());
    ASSERT_EQ(logLevel, GetParam());
}

TEST_P(LoggingTestWithLevels, SeverityInThreadCallback)
{
    static Logging::LogLevel logLevel;

    Logging::CallbackFunction logCallback = [](const char*, int level, void*) -> void {
        logLevel = static_cast<Logging::LogLevel>(level);
    };

    Logging::SetThreadCallback({logCallback, nullptr});
    logLevel = Logging::LogLevel::NotSet;

    Logging::Log("Test", GetParam());
    ASSERT_EQ(logLevel, GetParam());
}

TEST_P(LoggingTestWithLevels, LogLevelGlobal)
{
    Logging::SetGlobalLogLevel(GetParam());
    if (GetParam() == Logging::LogLevel::NotSet)
    {
        EXPECT_EQ(Logging::GetLogLevel(), Logging::m_defaultLogLevel);
    }
    else
    {
        EXPECT_EQ(Logging::GetLogLevel(), GetParam());
    }
}

TEST_P(LoggingTestWithLevels, LogLevelThread)
{
    Logging::SetThreadLogLevel(GetParam());
    if (GetParam() == Logging::LogLevel::NotSet)
    {
        EXPECT_EQ(Logging::GetLogLevel(), Logging::m_defaultLogLevel);
    }
    else
    {
        EXPECT_EQ(Logging::GetLogLevel(), GetParam());
    }
}

TEST_P(LoggingTestWithLevels, LogLevelThreadAndGlobal)
{
    Logging::SetGlobalLogLevel(Logging::LogLevel::Fatal);
    Logging::SetThreadLogLevel(GetParam());
    if (GetParam() == Logging::LogLevel::NotSet)
    {
        // Fallback to global
        EXPECT_EQ(Logging::GetLogLevel(), Logging::LogLevel::Fatal);
    }
    else
    {
        EXPECT_EQ(Logging::GetLogLevel(), GetParam());
    }
}

INSTANTIATE_TEST_SUITE_P(LogLevel, LoggingTestWithLevels, testing::Values(Logging::LogLevel::NotSet, Logging::LogLevel::Trace, Logging::LogLevel::Debug, Logging::LogLevel::Information, Logging::LogLevel::Warning, Logging::LogLevel::Error, Logging::LogLevel::Fatal));
