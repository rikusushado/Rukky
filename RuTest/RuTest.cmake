set(RuTest_dir "${CMAKE_SOURCE_DIR}/RuTest")

add_executable(RuTest_triangle)

include_directories("${RuCore_dir}/include")


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	include("${RuTest_dir}/cmake/build_linux.cmake")

else()
	target_sources(RuCore PRIVATE "${RuTest_dir}/source/void.c")
	message(FATAL_ERROR "Failed to build RuTest: Unsupported OS: ${CMAKE_SYSTEM_NAME}")
endif()
