if(NOT ENABLE_QT OR NOT ENABLE_PULSEAUDIO)
    return()
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(projectM-pulseaudio
        ConfigFile.h
        ConfigFile.cpp
        PulseDeviceChooserDialog.ui
        qprojectM-pulseaudio.cpp
        QPulseAudioDeviceChooser.cpp
        QPulseAudioDeviceChooser.hpp
        QPulseAudioDeviceModel.cpp
        QPulseAudioDeviceModel.hpp
        QPulseAudioThread.cpp
        QPulseAudioDeviceModel.hpp
        )

target_compile_definitions(projectM-pulseaudio
        PRIVATE
        PROJECTM_PREFIX="${CMAKE_INSTALL_PREFIX}"
        )

# Application includes a header file from libprojectM with its full path in the source dir.
target_include_directories(projectM-pulseaudio
        PRIVATE
        "${CMAKE_SOURCE_DIR}"
        )

target_link_libraries(projectM-pulseaudio
        PRIVATE
        projectM-qt
        Pulseaudio::Pulseaudio
        ${CMAKE_DL_LIBS}
        )

install(TARGETS projectM-pulseaudio
        RUNTIME DESTINATION "${PROJECTM_BIN_DIR}"
        COMPONENT Applications
        )

install(FILES "${CMAKE_SOURCE_DIR}/src/projectM-qt/images/icons/prjm16-transparent.svg"
        DESTINATION "share/icons/hicolor/scalable/apps"
        COMPONENT Applications
        RENAME projectM.svg
        )

install(FILES projectM-pulseaudio.desktop
        DESTINATION "share/applications"
        COMPONENT Applications
        )

install(FILES projectM-pulseaudio.1
        DESTINATION "share/man/man1"
        COMPONENT Applications
        )
