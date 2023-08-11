# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16.0)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "24")
set(RELEASE_SERVICE_VERSION_MINOR "01")
set(RELEASE_SERVICE_VERSION_MICRO "85")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(audiotube VERSION ${RELEASE_SERVICE_VERSION})

include(FeatureSummary)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION "5.15")
set(KF_MIN_VERSION "5.81.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

if (QT_MAJOR_VERSION STREQUAL "6")
	set(QT_MIN_VERSION "6.4.0")
	set(KF_MIN_VERSION "5.240.0")
	set(KF_MAJOR_VERSION "6")
else()
	set(KF_MAJOR_VERSION "5")
endif()

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMPoQmTools)
include(ECMAddTests)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX AUDIOTUBE
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

kde_enable_exceptions()

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Qml QuickControls2 Svg Sql Widgets Multimedia Concurrent DBus)
find_package(KF6 REQUIRED COMPONENTS Kirigami2 I18n CoreAddons Crash WindowSystem)
find_package(pybind11 REQUIRED)
find_package(Ytdlp REQUIRED RUNTIME)
find_package(YTMusicAPI 1.0.2 REQUIRED RUNTIME)
find_package(KF6KirigamiAddons 0.6.0 REQUIRED)
find_package(FutureSQL${QT_MAJOR_VERSION} REQUIRED)
find_package(QCoro${QT_MAJOR_VERSION} 0.10.0 REQUIRED COMPONENTS Core Network Qml)

ki18n_install(po)

add_subdirectory(src)

install(PROGRAMS org.kde.audiotube.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.audiotube.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.audiotube.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
