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
add_subdirectory(qtmpris)

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
    thumbnailsource.cpp
    abstractytmusicmodel.cpp
    multiiterableview.h
    library.cpp
    localplaylistmodel.cpp
    localplaylistsmodel.cpp
    playlistimporter.cpp
    resources.qrc
    blur.cpp
    clipboard.cpp
    lyricsmodel.cpp
)

target_link_libraries(audiotube
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
    Qt::Svg
    Qt::Widgets
    Qt::Concurrent
    Qt::Multimedia
    KF6::I18n
    KF6::CoreAddons
    KF6::Crash
    KF6::WindowSystem
    FutureSQL6::FutureSQL
    QCoro6::Core
    qtmprisdeclarative
    ytm
)

target_compile_definitions(audiotube PRIVATE
    -DAUDIOTUBE_VERSION_STRING="${RELEASE_SERVICE_VERSION}"
    -DQT_NO_KEYWORDS -DQT_NO_URL_CAST_FROM_STRING)

install(TARGETS audiotube ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
