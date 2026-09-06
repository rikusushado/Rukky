
#include "rukky/rukky.h"
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<X11/Xlib.h>
#include<X11/Xatom.h>

int rukky_main_loop (
	RukkyWindow win,
	uint fps,
	uintptr_t func,
	void* param
) {
	unsigned char* buffer = malloc(win.width * win.height * 3);
	
	Visual* vis = DefaultVisual(win.dpy, win.scn);
	printf("red_mask   = 0x%lx\n", vis->red_mask);
	printf("green_mask = 0x%lx\n", vis->green_mask);
	printf("blue_mask  = 0x%lx\n", vis->blue_mask);

	
	for (int i = 0; i < win.width * win.height; i++) {
		buffer[i + 0] = 0;   // blue
		buffer[i + 1] = 0;   // green
		buffer[i + 2] = 255; // red
    }

	XImage* img = XCreateImage(
			win.dpy,
			DefaultVisual(win.dpy, win.scn),
			24,
			ZPixmap,
			0,
			(char*)buffer,
			win.width,
			win.height,
			32,
			0
		);
	
	GC gc = DefaultGC(win.dpy, win.scn);
    XPutImage(win.dpy, win.self, gc, img, 0, 0, 0, 0, win.width, win.height);
	
    XEvent ev;
    while (1) {
        XNextEvent(win.dpy, &ev);
        if (ev.type == KeyPress)
            break;
    }

    free(buffer);       // frees pixel data
    XCloseDisplay(win.dpy);

	return 0;
}
