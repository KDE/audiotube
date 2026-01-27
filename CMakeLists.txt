# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16.0)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "12")
set(RELEASE_SERVICE_VERSION_MICRO "2")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(audiotube VERSION ${RELEASE_SERVICE_VERSION})

include(FeatureSummary)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION "6.4.0")
set(KF_MIN_VERSION "6.5.0")
set(KF_MAJOR_VERSION "6")

find_package(ECM 6.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMPoQmTools)
include(ECMAddTests)
include(ECMQmlModule)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX AUDIOTUBE
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

kde_enable_exceptions()

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Qml QuickControls2 Svg Sql Widgets Multimedia Concurrent DBus)
find_package(KF6 REQUIRED COMPONENTS Kirigami I18n CoreAddons Crash WindowSystem IconThemes)
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)
find_package(Ytdlp REQUIRED RUNTIME)
find_package(YTMusicAPI 1.10.2 REQUIRED RUNTIME)
find_package(KF6KirigamiAddons 0.11.0 REQUIRED)
find_package(FutureSQL6 REQUIRED)
find_package(QCoro6 0.10.0 REQUIRED COMPONENTS Core)

if(NOT TARGET Qt6::QGstreamerMediaPlugin)
  message(FATAL_ERROR "QtMultimedia gstreamer backend not found. AudioTube only works with the gstreamer backend.")
endif()

ki18n_install(po)

add_subdirectory(src)

install(PROGRAMS org.kde.audiotube.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.audiotube.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.audiotube.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
