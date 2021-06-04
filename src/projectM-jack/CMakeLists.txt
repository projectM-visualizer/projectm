if(NOT ENABLE_JACK)
    return()
endif()

if(ENABLE_QT)
    include(projectM-jack-qt.cmake)
endif()

if(ENABLE_SDL)
    include(projectM-jack-sdl.cmake)
endif()
