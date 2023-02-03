# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: BSD-2-Clause

add_library(ytm STATIC
    ytmusic.cpp
)

target_link_libraries(ytm PUBLIC pybind11::embed)
target_include_directories(ytm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(ytm PRIVATE -DRANDALL_WAS_HERE)

add_library(asyncdatabase STATIC
    asyncdatabase.cpp
    asyncdatabase_p.h
)

target_include_directories(asyncdatabase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(asyncdatabase PUBLIC Qt::Sql)

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
    autolistmodel.h
    multiiterableview.h
    library.cpp
    resources.qrc
    blur.cpp
)

target_link_libraries(audiotube
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
    Qt::Svg
    Qt::Widgets
    KF5::I18n
    KF5::CoreAddons
    KF5::Crash
    KF5::WindowSystem
    Desert::desert
    ytm
    asyncdatabase
)

target_compile_definitions(audiotube PRIVATE -DQT_NO_KEYWORDS -DQT_NO_URL_CAST_FROM_STRING)

install(TARGETS audiotube ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
