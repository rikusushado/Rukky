add_executable(rukky_triangle_test)

include_directories("include")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_sources(
		rukky_triangle_test
		PRIVATE
			"source/tests/triangle_Linux.c"
	)

    target_link_options(rukky_triangle_test PRIVATE "-Wl,-rpath=\$ORIGIN")

	target_link_libraries(
		rukky_triangle_test
		PRIVATE
			rukky_core
	)
endif()
