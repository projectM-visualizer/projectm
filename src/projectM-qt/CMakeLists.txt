if(NOT ENABLE_QT)
    return()
endif()

# Automatically create .moc files and run the UIC and resource compilers
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_library(projectM-qt STATIC
        application.qrc
        configfile.cpp
        configfile.hpp
        nullable.hpp
        qplaylistfiledialog.cpp
        qplaylistfiledialog.hpp
        qplaylistmodel.cpp
        qplaylistmodel.hpp
        qplaylisttableview.hpp
        qpreseteditordialog.cpp
        qpreseteditordialog.hpp
        qpreseteditordialog.ui
        qpresetfiledialog.hpp
        qpresettextedit.cpp
        qpresettextedit.hpp
        qprojectm.hpp
        qprojectm_mainwindow.cpp
        qprojectm_mainwindow.hpp
        qprojectm_mainwindow.ui
        qprojectmconfigdialog.cpp
        qprojectmconfigdialog.hpp
        qprojectmconfigdialog.ui
        qprojectmwidget.hpp
        qxmlplaylisthandler.hpp
        )

target_include_directories(projectM-qt
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

target_link_libraries(projectM-qt
        PUBLIC
        projectM::libprojectM
        ${QT_LINK_TARGETS}
        )
