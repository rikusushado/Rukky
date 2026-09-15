#include "rukky/core.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool RukkyNewWindow(
    uptr** window, 
    s32 x, s32 y,
    u32 width, u32 height,
    u32 border_width,
    char* title
) {
    *window = calloc(13, sizeof(uintptr_t));
    if (!*window) return false;
    
    (*window)[1] = (uptr)XOpenDisplay(NULL);
    if (!(*window)[1]) fatal("cannot open display");

    (*window)[2] = (uptr)DefaultScreen((Display*)(*window)[1]);

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
    GLXFBConfig *fbconfigs = glXChooseFBConfig((Display*)(*window)[1], (int)(*window)[2], fbAttribs, &fbcount);
    if (!fbconfigs || fbcount == 0) fatal("no FBConfig");

    GLXFBConfig fbconfig = fbconfigs[0];

    XVisualInfo *vi = glXGetVisualFromFBConfig((Display*)(*window)[1], fbconfig);
    if (!vi) fatal("no visual");

    (*window)[3] = (uptr)malloc(sizeof(XWindowAttributes));
   
    XSetWindowAttributes* swa = (XSetWindowAttributes*)(*window)[3]; 
    swa->colormap = XCreateColormap((Display*)(*window)[1], RootWindow((Display*)(*window)[1], vi->screen), vi->visual, AllocNone);
    swa->event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

    (*window)[4] = (uptr)malloc(sizeof(Window));

    Window* windowX11 = (Window*)(*window)[4]; 

    *windowX11 = XCreateWindow(
        (Display*)(*window)[1],
        RootWindow((Display*)(*window)[1], vi->screen),
        (*window)[5] = x, (*window)[6] = y, 
        (*window)[7] = width, (*window)[8] = height, 
        (*window)[9] = border_width,
        vi->depth,
        InputOutput,
        vi->visual,
        CWColormap | CWEventMask,
        swa
    );

    (*window)[10] = (uptr)malloc(sizeof(title));
    strncpy((char*)(*window)[10], (char*)title, sizeof(title) - 1);

    XStoreName((Display*)(*window)[1], *windowX11, (char*)(*window)[10]);
    XMapWindow((Display*)(*window)[1], *windowX11);

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

    (*window)[11] = (uptr)malloc(sizeof(GLXContext));
    GLXContext* contextGL = (GLXContext*)(*window)[11];
    if (glXCreateContextAttribsARB) {
        *contextGL = glXCreateContextAttribsARB((Display*)(*window)[1], fbconfig, 0, True, contextAttribs);
    } else {
        *contextGL = glXCreateNewContext((Display*)(*window)[1], fbconfig, GLX_RGBA_TYPE, 0, True);
    }

    if (!*contextGL) fatal("failed to create GL context");

    (*window)[12] = (uptr)malloc(sizeof(GLXWindow));

    GLXWindow* windowGLX = (GLXWindow*)(*window)[12]; 
    *windowGLX = glXCreateWindow((Display*)(*window)[1], fbconfig, *windowX11, NULL);
    glXMakeContextCurrent((Display*)(*window)[1], *windowGLX, *windowGLX, *contextGL);
    
    glViewport((s32)(*window)[5], (s32)(*window)[6], (u32)(*window)[7], (u32)(*window)[8]);

    XFlush((Display*)(*window)[1]);
    
    return true;
}
