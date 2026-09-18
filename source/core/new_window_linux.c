#include "rukky/core.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool RukkyNewWindow(
    RukkyWindow* window, 
    s32 x, s32 y,
    u32 width, u32 height,
    u32 border_width,
    char* title
) {
    window->display = XOpenDisplay(NULL);
    if (!window->display) fatal("cannot open display");

    window->screen = DefaultScreen(window->display);

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

    int fbcount;
    GLXFBConfig *fbconfigs = glXChooseFBConfig(window->display, window->screen, fbAttribs, &fbcount);
    if (!fbconfigs || fbcount == 0) fatal("no FBConfig");

    GLXFBConfig fbconfig = fbconfigs[0];

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
        vi->depth,
        InputOutput,
        vi->visual,
        CWColormap | CWEventMask,
        &window->set_window_attributes
    );

    XStoreName(window->display, window->selfX11, window->title = title);
    XMapWindow(window->display, window->selfX11);

    typedef GLXContext (*glXCreateContextAttribsARBProc)(
        Display*, GLXFBConfig, GLXContext, Bool, const int*
    );
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
