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
    void *handle = dlopen("", RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return false;
    }

    dlerror();

    (*function_array) = calloc(21, sizeof(uptr));

    dlfuncptr(function_array, 0, "wl_display_connect");
    dlfuncptr(function_array, 1, "wl_display_get_registry");
    dlfuncptr(function_array, 2, "wl_registry_add_listener");
    dlfuncptr(function_array, 3, "wl_display_roundtrip");
    dlfuncptr(function_array, 4, "wl_display_disconnect");
    dlfuncptr(function_array, 5, "wl_compositor_create_surface");
    dlfuncptr(function_array, 6, "wl_egl_window_create");
    dlfuncptr(function_array, 7, "wl_surface_destroy");
    dlfuncptr(function_array, 8, "eglGetDisplay");
    dlfuncptr(function_array, 9, "wl_egl_window_destroy");
    dlfuncptr(function_array, 10, "eglInitialize");
    dlfuncptr(function_array, 11, "eglBindAPI");
    dlfuncptr(function_array, 12, "eglChooseConfig");
    dlfuncptr(function_array, 13, "eglTerminate");
    dlfuncptr(function_array, 14, "eglCreateContext");
    dlfuncptr(function_array, 15, "eglCreateWindowSurface");
    dlfuncptr(function_array, 16, "eglDestroyContext");
    dlfuncptr(function_array, 17, "eglMakeCurrent");
    dlfuncptr(function_array, 18, "eglDestroySurface");
    dlfuncptr(function_array, 19, "wl_display_dispatch_pending");
    dlfuncptr(function_array, 20, "wl_display_dispatch");
    dlfuncptr(function_array, 21, "eglSwapBuffers");

    return true;
}

bool RukkyLoadLibraries(const char* library_name) {

}