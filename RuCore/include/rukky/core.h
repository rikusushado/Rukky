#pragma once

#define GL_GLEXT_PROTOTYPES

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define RUKKY_PUBLIC __attribute__((visibility("default")))

typedef uint32_t u32;
typedef int32_t s32;
typedef uintptr_t uptr;

typedef float f32;

void fatal(const char *msg);

RUKKY_PUBLIC bool RukkyNewWindow(
    uptr** window, 
    s32 x, s32 y,
    u32 width, u32 height,
    u32 border_width,
    char* title
);


RUKKY_PUBLIC void RukkyApplicationLoop(
    uptr** window, // Must be the window you want to draw in
    u32 usec_refresh_time, // Set a refresh time in micro seconds to limit resources 
    uptr callable, // Must be a function or `0`
    uptr* parameter, // Callable parameter or NULL
    s32 sleep_frames // Run callable every X frame ammount (Set to `-1` if no callable is provided) 
);

