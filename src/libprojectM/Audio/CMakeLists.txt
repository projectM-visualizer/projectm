
add_library(Audio OBJECT
        AudioConstants.hpp
        MilkdropFFT.cpp
        MilkdropFFT.hpp
        FrameAudioData.hpp
        PCM.cpp
        PCM.hpp
        Loudness.cpp
        Loudness.hpp
        WaveformAligner.cpp
        WaveformAligner.hpp
        )

target_include_directories(Audio
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        )

target_link_libraries(Audio
        PUBLIC
        libprojectM::API
        )

if(BUILD_SHARED_LIBS)
    if(ENABLE_CXX_INTERFACE)
        target_compile_definitions(Audio
                PRIVATE
                projectM_api_EXPORTS
                )
    else()
        target_compile_definitions(Audio
                PRIVATE
                PROJECTM_CXX_STATIC_DEFINE
                )
    endif()
endif()
