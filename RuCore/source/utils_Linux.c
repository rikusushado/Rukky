#include <rukky/core.h>
#include <dlfcn.h>

#include <stdio.h>

void fatal(const char *msg) {
    fprintf(stderr, "Fatal: %s\n", msg);
    exit(1);
}

#define dlfuncptr(array, index, symbol) \
    (*array)[index] = dlsym(handle, symbol)

bool RukkyLoadWayland(uptr** function_array) {
    void *handle = dlopen("/lib/x86_64-linux-gnu/libwayland-egl.so", RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return false;
    }

    dlerror();

    (*function_array) = calloc(21, sizeof(uptr));

    dlfuncptr(function_array, 0, "eglBindAPI"); // New Window
    dlfuncptr(function_array, 1, "eglChooseConfig"); // New Window
    dlfuncptr(function_array, 2, "eglCreateWindowSurface"); // New Window
    dlfuncptr(function_array, 3, "eglCreateContext"); // New Window
    dlfuncptr(function_array, 4, "eglDestroyContext"); // New Window & Loop
    dlfuncptr(function_array, 5, "eglDestroySurface"); // New Window & Loop
    dlfuncptr(function_array, 6, "eglGetDisplay"); // New Window
    dlfuncptr(function_array, 7, "eglInitialize"); // New Window
    dlfuncptr(function_array, 8, "eglMakeCurrent"); // New Window & Loop
    dlfuncptr(function_array, 9, "eglSwapBuffers"); // Loop
    dlfuncptr(function_array, 10, "eglTerminate"); // New Window & Loop
    dlfuncptr(function_array, 11, "wl_compositor_create_surface"); // New Window
    dlfuncptr(function_array, 12, "wl_display_connect"); // New Window
    dlfuncptr(function_array, 13, "wl_display_disconnect"); // New Window & Loop
    dlfuncptr(function_array, 14, "wl_display_dispatch"); // Loop
    dlfuncptr(function_array, 15, "wl_display_dispatch_pending"); // Loop
    dlfuncptr(function_array, 16, "wl_display_get_registry"); // New Window
    dlfuncptr(function_array, 17, "wl_display_roundtrip"); // New Window
    dlfuncptr(function_array, 18, "wl_egl_window_create"); // New Window
    dlfuncptr(function_array, 19, "wl_egl_window_destroy"); // New Window & Loop
    dlfuncptr(function_array, 20, "wl_registry_add_listener"); // New Window
    dlfuncptr(function_array, 21, "wl_surface_destroy"); // New Window & Loop

    return true;
}

bool RukkyLoadX11(uptr** function_array) {
    void *handle = dlopen("/lib/x86_64-linux-gnu/libX11.so", RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return false;
    }

    dlerror();

    (*function_array) = calloc(22, sizeof(uptr));

    dlfuncptr(function_array, 0, "XOpenDisplay"); // New Window
    dlfuncptr(function_array, 1, "glXChooseFBConfig"); // New Window
    dlfuncptr(function_array, 2, "glXGetVisualFromFBConfig"); // New Window
    dlfuncptr(function_array, 3, "XCreateColormap"); // New Window
    dlfuncptr(function_array, 4, "XCreateWindow"); // New Window
    dlfuncptr(function_array, 5, "XStoreName"); // New Window
    dlfuncptr(function_array, 6, "XMapWindow"); // New Window
    dlfuncptr(function_array, 7, "glXGetProcAddress"); // New Window
    dlfuncptr(function_array, 8, "glXCreateNewContext"); // New Window
    dlfuncptr(function_array, 9, "glXCreateWindow"); // New Window
    dlfuncptr(function_array, 10, "glXMakeContextCurrent"); // New Window & Loop
    dlfuncptr(function_array, 11, "XFlush"); // New Window
    dlfuncptr(function_array, 12, "XInternAtom"); // Loop
    dlfuncptr(function_array, 13, "XSetWMProtocols"); // Loop
    dlfuncptr(function_array, 14, "XPending"); // Loop
    dlfuncptr(function_array, 15, "XNextEvent"); // Loop
    dlfuncptr(function_array, 16, "glXSwapBuffers"); // Loop
    dlfuncptr(function_array, 17, "glXDestroyWindow"); // Loop
    dlfuncptr(function_array, 18, "glXDestroyContext"); // Loop
    dlfuncptr(function_array, 19, "XDestroyWindow"); // Loop
    dlfuncptr(function_array, 20, "XFreeColormap"); // Loop
    dlfuncptr(function_array, 21, "XCloseDisplay"); // Loop

    return true;
}


bool RukkyLoadLibraries(const char* library_name) {

}