#pragma once


#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define RUKKY_PUBLIC __attribute__((visibility("default")))

typedef uint32_t u32;
typedef int32_t s32;
typedef uintptr_t uptr;

typedef struct {
	Window self;
	Display* display;
	int screen;
	s32 x;
	s32 y;
	u32 width;
	u32 height;
	u32 border_width;
	char* title;
} RukkyWindow;

RUKKY_PUBLIC bool RukkyNewWindow(
    RukkyWindow* window, 
    s32 x, s32 y,
    u32 width, u32 height,
    u32 border_width,
    char* title
);


typedef struct {
	VkInstanceCreateInfo info;
	VkInstance instance;
} RukkyApplication;

RUKKY_PUBLIC bool RukkyNewApplication(
    RukkyApplication* application,
    char* application_name,
    char* engine_name
);

RUKKY_PUBLIC void RukkyApplicationLoop(
    RukkyApplication* application,
    RukkyWindow* window,
    u32 framerate,
    uptr callable, // Must be a function or `0`
    void* parameter, // Callable parameter or NULL
    s32 sleep_frames // Run callable every X frame ammount (Set to `-1` if no callable is provided) 
);

RUKKY_PUBLIC bool createRenderPass(VkRenderPass* renderpass, VkDevice* device, VkFormat swapChainImageFormat);
