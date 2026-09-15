#include "rukky/core.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool RukkyNewWindow(
    uptr* window, 
    int* framebuffer_attributes,
    s32 x, s32 y,
    u32 width, u32 height,
    u32 border_width,
    char* title
) {
<<<<<<< HEAD
    window->display = XOpenDisplay(NULL);
    if (!window->display) fatal("cannot open display");
=======
    window = calloc(6, sizeof(uptr)); 	

    window[1] = (uptr)XOpenDisplay(NULL);

    if (!window[1]) fatal("cannot open display");
>>>>>>> 2887119 (WIP: Core features)

    window[2] = (uptr)DefaultScreen(window[1]);

<<<<<<< HEAD
=======
    /*
>>>>>>> 2887119 (WIP: Core features)
    int fbAttribs[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,   GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE,      8,
        GLX_GREEN_SIZE,    8,
        GLX_BLUE_SIZE,     8,
        GLX_ALPHA_SIZE,    8,
        GLX_DEPTH_SIZE,    24,
        GLX_DOUBLEBUFFER,  True,
        None
    };
<<<<<<< HEAD

    int fbcount;
    GLXFBConfig *fbconfigs = glXChooseFBConfig(window->display, window->screen, fbAttribs, &fbcount);
=======
    */

    int fbcount;
    GLXFBConfig *fbconfigs = glXChooseFBConfig((Display*)window[1], (int)window[2], framebuffer_attributes, &fbcount);
>>>>>>> 2887119 (WIP: Core features)
    if (!fbconfigs || fbcount == 0) fatal("no FBConfig");

    GLXFBConfig fbconfig = fbconfigs[0];

<<<<<<< HEAD
    XVisualInfo *vi = glXGetVisualFromFBConfig(window->display, fbconfig);
    if (!vi) fatal("no visual");

    window->set_window_attributes.colormap = XCreateColormap(window->display, RootWindow(window->display, vi->screen), vi->visual, AllocNone);
    window->set_window_attributes.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

    window->selfX11 = XCreateWindow(
        window->display,
        RootWindow(window->display, vi->screen),
        window->x = x, window->y = y, 
        window->width = width, window->height = height, 
        window->border_width = border_width,
=======
    XVisualInfo *vi = glXGetVisualFromFBConfig((Display*)window[1], fbconfig);
    if (!vi) fatal("no visual");

    window[3] = (uptr)malloc(sizeof(XWindowAttributes));
    (XWindowAttributes*)window[3]->colormap = XCreateColormap(window[1], RootWindow(window[1], vi->screen), vi->visual, AllocNone);
    (XWindowAttributes*)window[3].event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

	
    window[4] = (uptr)malloc(sizeof(Window));
    *(Window*)window[4] = XCreateWindow(
        (Display*)window[1],
        RootWindow((Display*)window[1], vi->screen),
        window[5] = (uptr)x, window[6] = (uptr)y, 
        window[7] = (uptr)width, window[8] = (uptr)height, 
        window[9] = (uptr)border_width,
>>>>>>> 2887119 (WIP: Core features)
        vi->depth,
        InputOutput,
        vi->visual,
        CWColormap | CWEventMask,
<<<<<<< HEAD
        &window->set_window_attributes
    );

    XStoreName(window->display, window->selfX11, window->title = title);
    XMapWindow(window->display, window->selfX11);

    typedef GLXContext (*glXCreateContextAttribsARBProc)(
        Display*, GLXFBConfig, GLXContext, Bool, const int*
    );
=======
	(XWindowAttributes)window[3]
    );

    window[10] = malloc(sizeof(title));
    memcpy(title, window, sizeof(title));

    XStoreName((Display*)window[1], *(Window*)window[4], (char*)window[10]);
    XMapWindow((Display*)window[1], *(Window*)window[4]);

    typedef GLXContext (*glXCreateContextAttribsARBProc)(
        Display*, GLXFBConfig, GLXContext, Bool, const int*
    );

>>>>>>> 2887119 (WIP: Core features)
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB =
        (glXCreateContextAttribsARBProc)
        glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

    int contextAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    if (glXCreateContextAttribsARB) {
        window->context = glXCreateContextAttribsARB(window->display, fbconfig, 0, True, contextAttribs);
    } else {
        // Fallback: old-style context (may not be true core profile)
        window->context = glXCreateNewContext(window->display, fbconfig, GLX_RGBA_TYPE, 0, True);
    }

    if (!window->context) fatal("failed to create GL context");

    window->selfGL = glXCreateWindow(window->display, fbconfig, window->selfX11, NULL);
    glXMakeContextCurrent(window->display, window->selfGL, window->selfGL, window->context);
    
    glViewport(window->x, window->y, window->width, window->height);

    XFlush(window->display);
    
    return true;
}
