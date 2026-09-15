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
    int* framebuffer_attributes,
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

    int framebuffer_count;
    GLXFBConfig *framebuffer_config_array = glXChooseFBConfig((Display*)(*window)[1], (int)(*window)[2], framebuffer_attributes, &framebuffer_count);
    if (!framebuffer_config_array || framebuffer_count == 0) fatal("no FBConfig");

    GLXFBConfig framebuffer_config = framebuffer_config_array[0];

    XVisualInfo *visual_info = glXGetVisualFromFBConfig((Display*)(*window)[1], framebuffer_config);
    if (!visual_info) fatal("no visual");

    (*window)[3] = (uptr)malloc(sizeof(XWindowAttributes));
   
    XSetWindowAttributes* set_window_attributes = (XSetWindowAttributes*)(*window)[3]; 
    set_window_attributes->colormap = 
        XCreateColormap((Display*)(*window)[1], RootWindow((Display*)(*window)[1], visual_info->screen), visual_info->visual, AllocNone);
    set_window_attributes->event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

    (*window)[4] = (uptr)malloc(sizeof(Window));

    Window* window_x11 = (Window*)(*window)[4]; 

    *window_x11 = XCreateWindow(
        (Display*)(*window)[1],
        RootWindow((Display*)(*window)[1], visual_info->screen),
        (*window)[5] = x, (*window)[6] = y, 
        (*window)[7] = width, (*window)[8] = height, 
        (*window)[9] = border_width,
        visual_info->depth,
        InputOutput,
        visual_info->visual,
        CWColormap | CWEventMask,
        set_window_attributes
    );

    (*window)[10] = (uptr)malloc(sizeof(title));
    strncpy((char*)(*window)[10], (char*)title, sizeof(title) - 1);

    XStoreName((Display*)(*window)[1], *window_x11, (char*)(*window)[10]);
    XMapWindow((Display*)(*window)[1], *window_x11);

    typedef GLXContext (*glXCreateContextAttribsARBProc)(
        Display*, GLXFBConfig, GLXContext, Bool, const int*
    );
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB =
        (glXCreateContextAttribsARBProc)
        glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

    int context_attributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    (*window)[11] = (uptr)malloc(sizeof(GLXContext));
    GLXContext* context_opengl = (GLXContext*)(*window)[11];
    if (glXCreateContextAttribsARB) {
        *context_opengl = glXCreateContextAttribsARB((Display*)(*window)[1], framebuffer_config, 0, True, context_attributes);
    } else {
        *context_opengl = glXCreateNewContext((Display*)(*window)[1], framebuffer_config, GLX_RGBA_TYPE, 0, True);
    }

    if (!*context_opengl) fatal("failed to create OpenGL context");

    (*window)[12] = (uptr)malloc(sizeof(GLXWindow));

    GLXWindow* window_opengl = (GLXWindow*)(*window)[12]; 
    *window_opengl = glXCreateWindow((Display*)(*window)[1], framebuffer_config, *window_x11, NULL);
    glXMakeContextCurrent((Display*)(*window)[1], *window_opengl, *window_opengl, *context_opengl);
    
    glViewport((s32)(*window)[5], (s32)(*window)[6], (u32)(*window)[7], (u32)(*window)[8]);

    XFlush((Display*)(*window)[1]);
    
    return true;
}
