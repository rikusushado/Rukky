#include "rukky/core.h"

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RukkyApplicationLoop(
<<<<<<< HEAD
    RukkyWindow* window,
    u32 framerate,
=======
    RukkyWindow* window, // Must be the window you want to draw in
    u32 usec_refresh_time, // Set a refresh time in micro seconds to limit resources 
>>>>>>> 2887119 (WIP: Core features)
    uptr callable, // Must be a valid function
    uptr* parameter, // Callable parameter or NULL
    s32 sleep_frames // Run callable every X frame ammount (Set to `-1` if no callable is provided) 
) {
    void (*fptr)(void*, char) = NULL;

    s32 countdown;
    s32 frame_amount = sleep_frames + 1;

    if (callable) {
        fptr = (void (*)(void*, char))callable;

        if (sleep_frames < 0) {
            frame_amount = 1;
<<<<<<< HEAD
        } else if (sleep_frames == 0) {
            frame_amount = 0;
=======
>>>>>>> 2887119 (WIP: Core features)
        } else {
            frame_amount = sleep_frames;
        }

        fptr(parameter, 'i');
        countdown = frame_amount;
<<<<<<< HEAD
=======

>>>>>>> 2887119 (WIP: Core features)
    } else {
        frame_amount = 0;
        countdown = -1;
    }

    Atom wm_delete_window = XInternAtom(
        window->display,
        "WM_DELETE_WINDOW",
        False
    ); 

    XSetWMProtocols(
        window->display,
	window->selfX11,
	&wm_delete_window,
	1
    );
    XEvent ev;
    int running = 1;
    while (running)
    {
        while (XPending(window->display)) {
            XNextEvent(window->display, &ev);
            if (ev.type == ClientMessage) {
                if ((Atom)ev.xclient.data.l[0] == wm_delete_window) {
                    running = 0;
                }
            } else if (ev.type == DestroyNotify) {
                running = 0;
            }
        }

        if (fptr) {
            if (frame_amount == 0) {
								fptr(parameter, 'l');
            } else {
                if (countdown <= 0) {
                    fptr(parameter, 'l');
                    countdown = frame_amount;
                } else {
                    --countdown;
                }
            }
        }

        glXSwapBuffers(window->display, window->selfGL);

	    usleep(1000000 / framerate);
    }
    
    if (fptr) fptr(parameter, 'e');

    glXMakeContextCurrent(window->display, None, None, NULL);
    glXDestroyWindow(window->display, window->selfGL);
    glXDestroyContext(window->display, window->context);
    XDestroyWindow(window->display, window->selfX11);
    XFreeColormap(window->display, window->set_window_attributes.colormap);
    XCloseDisplay(window->display);

}
