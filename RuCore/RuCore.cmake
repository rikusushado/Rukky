add_library(RuCore SHARED)

set(RuCore_dir "${CMAKE_SOURCE_DIR}/RuCore")

include_directories("${RuCore_dir}/include")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	include("${RuCore_dir}/cmake/build_linux.cmake")
endif()
