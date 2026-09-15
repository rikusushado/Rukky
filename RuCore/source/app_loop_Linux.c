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
    uptr** window, // Must be the window you want to draw in
    u32 usec_refresh_time, // Set a refresh time in micro seconds to limit resources 
    uptr callable, // Must be a valid function
    uptr* parameter, // Callable parameter or NULL
    s32 sleep_frames // Run callable every X frame ammount (Set to `-1` if no callable is provided) 
) {
    void (*fptr)(void*, char) = NULL;

    s32 countdown = -1;
    s32 frame_amount = sleep_frames + 1;

    if (callable) {
        fptr = (void (*)(void*, char))callable;

        printf("Callable: %lu\n", (uptr)fptr);

        if (sleep_frames < 0) {
            frame_amount = 1;
        } else if (sleep_frames == 0) {
            frame_amount = 0;
        } else {
            frame_amount = sleep_frames;
            countdown = frame_amount;
        }

        fptr(parameter, 'i');
    } else {
        frame_amount = 0;
        countdown = -1;
    }

    Atom wm_delete_window = XInternAtom(
        (Display*)(*window)[1],
        "WM_DELETE_WINDOW",
        False
    ); 

    XSetWMProtocols(
        (Display*)(*window)[1],
	    *(Window*)(*window)[4],
        &wm_delete_window,
        1
    );
    XEvent ev;
    int running = 1;
    while (running)
    {
        while (XPending((Display*)(*window)[1])) {
            XNextEvent((Display*)(*window)[1], &ev);
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

        glXSwapBuffers((Display*)(*window)[1], *(GLXWindow*)(*window)[12]);

	    usleep(usec_refresh_time);
    }
    
    if (fptr) fptr(parameter, 'e');

    glXMakeContextCurrent((Display*)(*window)[1], None, None, NULL);
    glXDestroyWindow((Display*)(*window)[1], *(GLXWindow*)(*window)[12]);
    glXDestroyContext((Display*)(*window)[1],*(GLXContext*)(*window)[11]);
    XDestroyWindow((Display*)(*window)[1], *(Window*)(*window)[4]);

    XSetWindowAttributes* swa = (XSetWindowAttributes*)(*window)[3];
    XFreeColormap((Display*)(*window)[1], swa->colormap);
    XCloseDisplay((Display*)(*window)[1]);

    free((void*)(*window)[3]);
    free((void*)(*window)[4]);
    free((void*)(*window)[10]);
    free((void*)(*window)[11]);
    free((void*)(*window)[12]);
}
