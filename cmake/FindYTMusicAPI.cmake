# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: BSD-2-Clause

find_package(Python3 REQUIRED COMPONENTS Interpreter)

execute_process(COMMAND ${Python3_EXECUTABLE} -c "import ytmusicapi" RESULT_VARIABLE YTMUSICAPI_CHECK_RESULT)

if (${YTMUSICAPI_CHECK_RESULT} EQUAL 0)
	set(YTMusicAPI_FOUND TRUE)
endif()
