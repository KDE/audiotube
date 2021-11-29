# SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
#
# SPDX-License-Identifier: BSD-2-Clause

find_package(Python3 REQUIRED COMPONENTS Interpreter)

execute_process(COMMAND ${Python3_EXECUTABLE} -c "import yt_dlp" RESULT_VARIABLE YTDLP_CHECK_RESULT)

if (${YTDLP_CHECK_RESULT} EQUAL 0)
	set(Ytdlp_FOUND TRUE)
endif()
