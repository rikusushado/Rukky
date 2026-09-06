#include "rukky/core.h"

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan.h>

bool RukkyNewApplication(
    RukkyApplication* application,
    char* application_name,
    char* engine_name
) {
    application = malloc(sizeof(RukkyApplication));
    const char *extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,

        VK_KHR_XLIB_SURFACE_EXTENSION_NAME
    };

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,

        .pApplicationName = application_name,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),

        .pEngineName = engine_name,
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),

        .apiVersion = VK_API_VERSION_1_0
    };

    application->info.sType =
	VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    application->info.pApplicationInfo = &appInfo;
    application->info.enabledExtensionCount = 2;
    application->info.ppEnabledExtensionNames = extensions;

    if (vkCreateInstance(
	&application->info,
	NULL,
	&application->instance
	) != VK_SUCCESS
    ) {
	#ifdef RUKKY_DEBUG
	fprintf(stderr, "Failed to create Vulkan instance\n");
	#endif
        
	return false;
    }

    return true;
}
