# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: BSD-2-Clause

add_library(ytm STATIC
    ytmusic.cpp
)

target_link_libraries(ytm PUBLIC pybind11::embed)
target_include_directories(ytm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(ytm PRIVATE -DRANDALL_WAS_HERE)

add_subdirectory(example)

add_executable(audiotube
    main.cpp
    asyncytmusic.cpp
    searchmodel.cpp
    albummodel.cpp
    videoinfoextractor.cpp
    artistmodel.cpp
    userplaylistmodel.cpp
    playlistmodel.cpp
    playlistutils.cpp
    errorhandler.cpp
    playerutils.cpp
    abstractytmusicmodel.cpp
    multiiterableview.h
    library.cpp
    resources.qrc
)

target_link_libraries(audiotube
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Svg
    KF5::I18n
    KF5::CoreAddons
    KF5::Crash
    ytm
    qpersistentdata
)
target_compile_definitions(audiotube PRIVATE -DQT_NO_KEYWORDS -DQT_NO_URL_CAST_FROM_STRING)

install(TARGETS audiotube ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
