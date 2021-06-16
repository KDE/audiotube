find_package(Python3 REQUIRED COMPONENTS Interpreter)

execute_process(COMMAND ${Python3_EXECUTABLE} -c "import youtube_dl" RESULT_VARIABLE YTDL_CHECK_RESULT)

if (${YTDL_CHECK_RESULT} EQUAL 0)
	set(YoutubeDL_FOUND TRUE)
endif()
