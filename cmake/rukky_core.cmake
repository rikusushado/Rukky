add_library(rukky_core SHARED)

include_directories("include")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_sources(
		rukky_core
		PRIVATE
			"source/core/new_window_X11.c"
			"source/core/app_loop_X11.c"
			"source/core/utils_Linux.c"
	)

	target_link_libraries(
		rukky_core
		PRIVATE
			X11
		PUBLIC
			GL
	)
endif()
