#include "rukky/core.h"

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan.h>

void RukkyApplicationLoop(
    RukkyApplication* application,
    RukkyWindow* window,
    u32 framerate,
    uptr callable, // Must be a function or `0`
    void* parameter, // Callable parameter or NULL
    s32 sleep_frames // Run callable every X frame ammount (Set to `-1` if no callable is provided) 
) {
    Atom wm_delete_window = XInternAtom(
        window->display,
	"WM_DELETE_WINDOW",
	False
    ); 

    XSetWMProtocols(
        window->display,
	window->self,
	&wm_delete_window,
	1
    );

    XEvent event;

    void (*fptr)(void*);

    s32 countdown;
    s32 frame_amount = sleep_frames + 1;

    if (frame_amount && callable)
    {
	fptr = (void*)callable;
    } else {
	countdown = -1;	
    }

    int running = 1;
    while (running)
    {
        XNextEvent(window->display, &event);

        if (event.type == ClientMessage)
	{
	    if ((Atom)event.xclient.data.l[0]
		== wm_delete_window)
	    {
                running = 0;
	    }
        }

	if (!countdown && fptr)
	{
	    fptr(parameter);
	    countdown = frame_amount;	
	}
	else if (countdown > 0)
	{
	    --countdown;
	}
    }
    
    vkDestroyInstance(application->instance, NULL);
    XDestroyWindow(window->display, window->self);
    XCloseDisplay(window->display);
}
