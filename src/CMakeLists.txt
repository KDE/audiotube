add_library(ytm STATIC
    ytmusic.cpp
)

target_link_libraries(ytm PUBLIC pybind11::embed)
target_include_directories(ytm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(example)

add_executable(youtube_music
    main.cpp
    asyncytmusic.cpp
    searchmodel.cpp
    resources.qrc
)

target_link_libraries(youtube_music
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Svg
    KF5::I18n
    ytm)

install(TARGETS youtube_music ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
