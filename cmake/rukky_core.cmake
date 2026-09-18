add_library(rukky_core)

target_include_directories(include)

target_sources(
	rukky_core
	PRIVATE
		"source/core/new_window_android.c"
		"source/core/app_loop_android.c"
		"source/core/utils_android.c"

		"source/core/new_window_bsd.c"
		"source/core/app_loop_bsd.c"
		"source/core/utils_bsd.c"

		"source/core/new_window_ios.c"
		"source/core/app_loop_ios.c"
		"source/core/utils_ios.c"

		"source/core/new_window_linux.c"
		"source/core/app_loop_linux.c"
		"source/core/utils_linux.c"
		
		"source/core/new_window_macos.c"
		"source/core/app_loop_macos.c"
		"source/core/utils_macos.c"

		"source/core/new_window_windows.c"
		"source/core/app_loop_windows.c"
		"source/core/utils_windows.c"
)

if (CMAKE_SYSTEM_NAME EQUAL "Android")
	target_link_libraries()
