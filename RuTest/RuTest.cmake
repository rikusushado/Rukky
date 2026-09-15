add_executable(RuTest_triangle)

include_directories("${CMAKE_SOURCE_DIR}/RuCore/include")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_sources(
		RuTest_triangle
		PRIVATE
			"${CMAKE_SOURCE_DIR}/RuTest/source/triangle_Linux.c"
	)

    target_link_options(RuTest_triangle PRIVATE "-Wl,-rpath=\$ORIGIN")

	target_link_libraries(
		RuTest_triangle
		PRIVATE
			RuCore
	)
endif()
