# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16.0)

project(audiotube)

include(FeatureSummary)

set(PROJECT_VERSION "22.02")
set(QT5_MIN_VERSION 5.15)
set(KF5_MIN_VERSION 5.81)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMPoQmTools)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX AUDIOTUBE
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

kde_enable_exceptions()

find_package(Qt5 ${QT5_MIN_VERSION} REQUIRED COMPONENTS Core Gui Qml QuickControls2 Svg)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Kirigami2 I18n CoreAddons Crash)
find_package(pybind11 REQUIRED)
find_package(Ytdlp REQUIRED RUNTIME)
find_package(YTMusicAPI REQUIRED RUNTIME)

add_subdirectory(src)
add_subdirectory(qpersistentdata)

install(PROGRAMS org.kde.audiotube.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.audiotube.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.audiotube.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
