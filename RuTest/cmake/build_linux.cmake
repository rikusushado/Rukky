	target_sources(
		RuTest_triangle
		PRIVATE
			"${RuTest_dir}/source/triangle_Linux.c"
	)

    target_link_options(RuTest_triangle PRIVATE "-Wl,-rpath=\$ORIGIN")

	target_link_libraries(
		RuTest_triangle
		PRIVATE
			RuCore
	)