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
    abstractshelfmodel.cpp
    homemodel.cpp
    exploremodel.cpp
    library.cpp
    localplaylistmodel.cpp
    localplaylistsmodel.cpp
    playlistimporter.cpp
    blur.cpp
    clipboard.cpp
    lyricsmodel.cpp
    foreigntypes.h
)

ecm_add_qml_module(audiotube URI org.kde.audiotube
    QML_FILES
        contents/ui/Main.qml
        contents/ui/AlbumPage.qml
        contents/ui/ArtistPage.qml
        contents/ui/PlayerFooter.qml
        contents/ui/SearchPage.qml
        contents/ui/PlaylistPage.qml
        contents/ui/ConfirmationMessage.qml
        contents/ui/LibraryPage.qml
        contents/ui/MinimizedPlayerControls.qml
        contents/ui/MaximizedPlayerPage.qml
        contents/ui/PlaybackHistory.qml
        contents/ui/SongMenu.qml
        contents/ui/ListHeader.qml
        contents/ui/RoundedImage.qml
        contents/ui/Sidebar.qml
        contents/ui/NavigationBar.qml
        contents/ui/SearchHistoryPage.qml
        contents/ui/SearchWithDropdown.qml
        contents/ui/ShareMenu.qml
        contents/ui/LocalPlaylistPage.qml
        contents/ui/LocalPlaylistsPage.qml
        contents/ui/PlaylistCover.qml
        contents/ui/dialogs/PlaylistDialog.qml
        contents/ui/dialogs/AddPlaylistDialog.qml
        contents/ui/dialogs/ImportPlaylistDialog.qml
        contents/ui/dialogs/RenamePlaylistDialog.qml
        contents/ui/HorizontalCoverView.qml
        contents/ui/AlbumCoverItem.qml
        contents/ui/HomeShelf.qml
        contents/ui/HeroHomeShelf.qml
        contents/ui/HomePage.qml
        contents/ui/BentoCard.qml
        contents/ui/HeroCard.qml
        contents/ui/ExplorePage.qml
        contents/ui/MoodPage.qml
        contents/ui/components/ActionButton.qml
        contents/ui/components/DoubleActionButton.qml
    RESOURCES
        migrations/2022-05-20-194850_init/down.sql
        migrations/2022-05-20-194850_init/up.sql
        migrations/2022-05-25-212054_playlists/down.sql
        migrations/2022-05-25-212054_playlists/up.sql
        resources/favourites_placeholder.svg
        resources/played_placeholder.svg
        resources/playlist_placeholder.svg
)

target_link_libraries(audiotube PRIVATE
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
    KF6::IconThemes
    FutureSQL6::FutureSQL
    QCoro6::Core
    qt-mpris
    ytm
)

target_compile_definitions(audiotube PRIVATE
    -DAUDIOTUBE_VERSION_STRING="${RELEASE_SERVICE_VERSION}"
    -DQT_NO_KEYWORDS -DQT_NO_URL_CAST_FROM_STRING)

install(TARGETS audiotube ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
