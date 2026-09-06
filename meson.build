project ('rukky', 'c')

cc = meson.get_compiler('c')
x11_dep = cc.find_library('X11',
	required : false)
vulkan_dep = cc.find_library('vulkan',
	required : false)
inc = include_directories('include')
librukky_core = shared_library('rukky-core',
	[
	'source/core/new_window.c',
        'source/core/new_app.c',
	'source/core/app_loop.c'
	],
	include_directories: inc,
	dependencies: [x11_dep, vulkan_dep],
	gnu_symbol_visibility : 'default',
	install : true
)

executable('rukky-blank-window',
	'source/test/blank_window.c',
	include_directories: inc,
	link_with: librukky_core
)
