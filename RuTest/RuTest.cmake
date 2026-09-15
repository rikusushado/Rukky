add_executable(RuTest_triangle)

include_directories("${RuCore_dir}/include")

set(RuTest_dir "${CMAKE_SOURCE_DIR}/RuTest")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	include("${RuTest_dir}/cmake/build_linux.cmake")
endif()
