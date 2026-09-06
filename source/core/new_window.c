#include "rukky/core.h"

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan.h>

bool RukkyNewWindow(
    RukkyWindow* window, 
    s32 x, s32 y,
    u32 width, u32 height,
    u32 border_width,
    char* title
) {
    window = malloc(sizeof(RukkyWindow));
    window->display = XOpenDisplay(NULL);

    if (!window->display) {
	fprintf(stderr, "Failed to open X display\n");

	free(window);
	window = NULL;

        return false;
    }

    window->screen = DefaultScreen(window->display);

    window->self = XCreateSimpleWindow(
	window->display, RootWindow(
	    window->display, window->screen),

        window->x = x, window->y = y,
	window->width = width, window->height = height,
	window->border_width = border_width,

        BlackPixel(window->display, window->screen),
        WhitePixel(window->display, window->screen)
    );

    XMapWindow(window->display, window->self);

    XStoreName(window->display, window->self, window->title = title);

    XFlush(window->display);
    
    return true;
}
