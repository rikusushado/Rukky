project ('rukky', 'c')

cc = meson.get_compiler('c')
x11_dep = cc.find_library('X11',
	required : false)
gl_dep = cc.find_library('GL',
	required : false)
xcb_dep = cc.find_library('xcb', required : true)

inc = include_directories('include')
librukky_core = shared_library('rukky-core',
	[
	'source/core/new_window.c',
	'source/core/app_loop.c',
	'source/core/util.c'
	],
	include_directories: inc,
	dependencies: [
        x11_dep,
        xcb_dep,
        gl_dep
    ],
	gnu_symbol_visibility : 'default',
	install : true
)

executable('rukky-blank-window',
	'source/test/blank_window.c',
	include_directories: inc,
	link_with: librukky_core
)

executable('rukky-triangle-window',
	'source/test/triangle_window.c',
	include_directories: inc,
	link_with: librukky_core,
		dependencies: [
        x11_dep,
        xcb_dep,
        gl_dep
    ],
)
