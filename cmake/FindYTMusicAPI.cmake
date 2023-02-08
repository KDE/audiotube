# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: BSD-2-Clause

find_package(Python3 REQUIRED COMPONENTS Interpreter)

execute_process(COMMAND ${Python3_EXECUTABLE} -c "import ytmusicapi; print(ytmusicapi.__version__)" RESULT_VARIABLE YTMUSICAPI_CHECK_RESULT OUTPUT_VARIABLE YTMusicAPI_VERSION)

string(STRIP ${YTMusicAPI_VERSION} YTMusicAPI_VERSION)

if (${YTMUSICAPI_CHECK_RESULT} EQUAL 0)
	set(YTMusicAPI_FOUND TRUE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YTMusicAPI
    FOUND_VAR
        YTMusicAPI_FOUND
    REQUIRED_VARS
        Python3_EXECUTABLE
    VERSION_VAR
        YTMusicAPI_VERSION
)
