#include "rukky/core.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

static void registry_global(
    void *data, struct wl_registry *registry,
    uint32_t name, const char *interface, uint32_t version
) {
    struct wl_compositor **compositor = data;
    if (strcmp(interface, "wl_compositor") == 0) {
        *compositor = wl_registry_bind(
            registry, name, &wl_compositor_interface, 1);
    }
}

static void registry_remove(
    void *data, struct wl_registry *registry, uint32_t name
) {
    (void)data; (void)registry; (void)name;
}

bool RukkyNewWindowWayland(
    uptr** window,
    int* attributes,
    s32 x, s32 y,
    u32 width, u32 height,
    u32 border_width,
    char* title
) {
    *window = calloc(13, sizeof(uptr));
    if (!*window) return false;

    struct wl_display *display = wl_display_connect(NULL);
    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland display\n");
        return false;
    }

    (*window)[1] = (uptr)display;

    struct wl_registry    *registry  = wl_display_get_registry(display);
    struct wl_compositor  *compositor = NULL;

    static const struct wl_registry_listener reg_listener = {
        registry_global,
        registry_remove
    };

    wl_registry_add_listener(registry, &reg_listener, &compositor);
    wl_display_roundtrip(display);

    if (!compositor) {
        fprintf(stderr, "No wl_compositor found\n");
        wl_display_disconnect(display);
        return false;
    }

    struct wl_surface *surface = wl_compositor_create_surface(compositor);
    if (!surface) {
        fprintf(stderr, "Failed to create wl_surface\n");
        wl_display_disconnect(display);
        return false;
    }

    (*window)[2] = (uptr)surface;

    struct wl_egl_window *egl_window =
    wl_egl_window_create(surface, width, height);
    if (!egl_window) {
        fprintf(stderr, "Failed to create wl_egl_window\n");
        wl_surface_destroy(surface);
        wl_display_disconnect(display);
        return false;
    }

    (*window)[3] = (uptr)egl_window;

    EGLDisplay egl_dpy = eglGetDisplay((EGLNativeDisplayType)display);
    if (egl_dpy == EGL_NO_DISPLAY) {
        fprintf(stderr, "eglGetDisplay failed\n");
        wl_egl_window_destroy(egl_window);
        wl_surface_destroy(surface);
        wl_display_disconnect(display);
        return false;
    }

    (*window)[4] = (uptr)egl_dpy;

    if (!eglInitialize(egl_dpy, NULL, NULL)) {
        fprintf(stderr, "eglInitialize failed\n");
        wl_egl_window_destroy(egl_window);
        wl_surface_destroy(surface);
        wl_display_disconnect(display);
        return false;
    }

    eglBindAPI(EGL_OPENGL_API);

    EGLConfig config;
    EGLint num_cfg;
    if (!eglChooseConfig(egl_dpy, attributes, &config, 1, &num_cfg) || num_cfg < 1) {
        fprintf(stderr, "eglChooseConfig failed\n");
        eglTerminate(egl_dpy);
        wl_egl_window_destroy(egl_window);
        wl_surface_destroy(surface);
        wl_display_disconnect(display);
        return false;
    }

    EGLint ctx_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 3,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_NONE
    };

    EGLContext ctx = eglCreateContext(egl_dpy, config, EGL_NO_CONTEXT, ctx_attribs);
    if (ctx == EGL_NO_CONTEXT) {
        fprintf(stderr, "eglCreateContext (3.3 core) failed\n");
        eglTerminate(egl_dpy);
        wl_egl_window_destroy(egl_window);
        wl_surface_destroy(surface);
        wl_display_disconnect(display);
        return false;
    }

    (*window)[5] = (uptr)ctx;

    EGLSurface egl_surf = eglCreateWindowSurface(
    egl_dpy, config, (EGLNativeWindowType)egl_window, NULL);
    if (egl_surf == EGL_NO_SURFACE) {
        fprintf(stderr, "eglCreateWindowSurface failed\n");
        eglDestroyContext(egl_dpy, ctx);
        eglTerminate(egl_dpy);
        wl_egl_window_destroy(egl_window);
        wl_surface_destroy(surface);
        wl_display_disconnect(display);
        return false;
    }

    (*window)[6] = (uptr)egl_surf;

    if (!eglMakeCurrent(egl_dpy, egl_surf, egl_surf, ctx)) {
        fprintf(stderr, "eglMakeCurrent failed\n");
        eglDestroySurface(egl_dpy, egl_surf);
        eglDestroyContext(egl_dpy, ctx);
        eglTerminate(egl_dpy);
        wl_egl_window_destroy(egl_window);
        wl_surface_destroy(surface);
        wl_display_disconnect(display);
        return false;
    }

    return true;
}

bool RukkyNewWindowX11(
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
        x, y, 
        width, height, 
        border_width,
        visual_info->depth,
        InputOutput,
        visual_info->visual,
        CWColormap | CWEventMask,
        set_window_attributes
    );

    XStoreName((Display*)(*window)[1], *window_x11, title);
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

    (*window)[5] = (uptr)malloc(sizeof(GLXContext));
    GLXContext* context_opengl = (GLXContext*)(*window)[11];
    if (glXCreateContextAttribsARB) {
        *context_opengl = glXCreateContextAttribsARB((Display*)(*window)[1], framebuffer_config, 0, True, context_attributes);
    } else {
        *context_opengl = glXCreateNewContext((Display*)(*window)[1], framebuffer_config, GLX_RGBA_TYPE, 0, True);
    }

    if (!*context_opengl) fatal("failed to create OpenGL context");

    (*window)[6] = (uptr)malloc(sizeof(GLXWindow));

    GLXWindow* window_opengl = (GLXWindow*)(*window)[6]; 
    *window_opengl = glXCreateWindow((Display*)(*window)[1], framebuffer_config, *window_x11, NULL);
    glXMakeContextCurrent((Display*)(*window)[1], *window_opengl, *window_opengl, *context_opengl);

    XFlush((Display*)(*window)[1]);
    
    return true;
}

bool RukkyNewWindow(
    uptr** window,
    int* attributes,
    s32 x, s32 y,
    u32 width, u32 height,
    u32 border_width,
    char* title
) {
    const char* session_type = getenv("XDG_SESSION_TYPE");

    if (session_type) return false;

    *window = calloc(13, sizeof(uptr));
    if (!*window) return false;

    (*window)[7] = (uptr)x;
    (*window)[8] = (uptr)y;
    (*window)[9] = (uptr)width;
    (*window)[10] = (uptr)height;
    (*window)[11] = (uptr)border_width;

    (*window)[12] = (uptr)malloc(sizeof(title));
    strncpy((char*)(*window)[10], (char*)title, sizeof(title) - 1);

    bool success;

    if (strcmp(session_type, "x11") == 0) {
        success = RukkyNewWindowX11(
            window,
            attributes,
            x, y,
            width, height,
            border_width,
            title
        );
    } else if (strcmp(session_type, "wayland") == 0) {
        success = RukkyNewWindowWayland(
            window,
            attributes,
            x, y,
            width, height,
            border_width,
            title
        );
    } else {
        fprintf(stderr, "Failed to create window: Session type is not X11 or Wayland.");
        free((void*)(*window)[12]);
        return false;
    }

    glViewport(x, y, width, height);

    return success;
}