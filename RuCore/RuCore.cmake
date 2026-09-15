set(RuCore_dir "${CMAKE_SOURCE_DIR}/RuCore")

add_library(RuCore SHARED)

include_directories("${RuCore_dir}/include")



if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	include("${RuCore_dir}/cmake/build_linux.cmake")
else()
	target_sources(RuCore PRIVATE "${RuCore_dir}/source/void.c")
	message(FATAL_ERROR "Failed to build RuCore: Unsupported OS: ${CMAKE_SYSTEM_NAME}")
endif()
