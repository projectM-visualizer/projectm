#include <projectM-4/Logging.hpp>
#include <projectM-4/ProjectM.hpp>

int main()
{
    auto logLevel = libprojectM::Logging::GetLogLevel();

    libprojectM::ProjectM pm;

    auto frameData = pm.PCM().GetFrameAudioData();

    return 0;
}
