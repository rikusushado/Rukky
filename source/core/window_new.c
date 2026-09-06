#include "rukky/rukky.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <vulkan/vulkan.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <syslog.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t i32;

typedef float f32;

#define array_count(array) (sizeof(array) / sizeof((array)[0]))

typedef struct
{    
	Window window;
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	u32 graphicsAndPresentQueueFamily;
	VkQueue graphicsAndPresentQueue;
	VkSwapchainKHR swapchain;
	VkFormat swapchainImageFormat;
	VkImage swapchainImages[2];
	VkImageView swapchainImageViews[2];
	VkExtent2D swapchainExtents;
} VulkanContext;

typedef struct{
	void *data;
	size_t size;
} LoadedFile;

LoadedFileload_entire_file(char *fileName)
{
	LoadedFile result = {0};
	
	FILE *handle;
	fopen_s(&handle, fileName, "rb");
	assert(handle);
	
	fseek(handle, 0, SEEK_END);
	result.size = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	assert(result.size > 0);
	
	result.data = malloc(result.size);
	assert(result.data);
	
	size_t bytesRead = fread(result.data, 1, result.size, handle);
	assert(bytesRead == result.size);
	
	fclose(handle);
	
	return result;
}

static bool globalRunning = true;

void handle_x11_events(Display *display, Atom wmDeleteMessage)
{
	while (XPending(display) > 0)
	{
		XEvent event;
		XNextEvent(display, &event)
		
		switch (event.type)
		{
		case ConfigureNotify:
		{
			XConfigureEvent xce = event.xconfigure;
			printf("Window resized\n");
		}
		break;
	
		case ClientMessage:
		{
			if(
				(Atom)event.xclient.data.l[0] ==
				wmDeleteMessage
			)
			{
				globalRunning = false;
			}
		}
		break;
		
		default:
			break;
		}
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
	void *userData
) {
	fprintf(stderr, "Vulkan Validation layer: %s\n", callbackData->pMessage);
	return VK_FALSE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
	void *userData
) {
	syslog(LOG_DEBUG, "Vulkan Validation layer: %s", callbackData->pMessage);
	return VK_FALSE;
}

VulkanContext
x11_init_vulkan(i32 windowX, i32 windowY, u32 windowWidth,                  u32 windowHeight, char *windowTitle)
{
	VulkanContext vk = {0};
	
	Display* display = XOpenDisplay(NULL);
	if (!display)
	{
		assert(!"Failed to open X display");
	}

	int screen = DefaultScreen(display);
	Window root = RootWindow(display, screen);

	XSetWindowAttributes attr;
	attr.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

	Window window = XCreateWindow(
		display,
		root,
		windowX, windowY,
		windowWidth, windowHeight,
		0,
		CopyFromParent,
		InputOutput,
		CopyFromParent,
		CWEventMask,
		&attr
	);

	if (!window)
	{
		assert(!"Failed to create window");
	}

	XStoreName(display, window, windowTitle);

	XSizeHints* hints = XAllocSizeHints();
	if (hints)
	{
		hints->flags = PMinSize | PMaxSize | PPosition;
		hints->min_width = windowWidth;
		hints->max_width = windowWidth;
		hints->min_height = windowHeight;
		hints->max_height = windowHeight;
		hints->x = windowX;
		hints->y = windowY;
		XSetWMNormalHints(display, window, hints);
		XFree(hints);
	}

	Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, window, &wmDeleteMessage, 1);

	XMapWindow(display, window);
	XFlush(display);

	vk.window = window;
	
	u32 propertyCount = 0;
	vkEnumerateInstanceLayerProperties(&propertyCount, NULL);
	assert(propertyCount <= 32);

	VkLayerProperties layerProperties[32];
	vkEnumerateInstanceLayerProperties(&propertyCount, layerProperties);

	char *validationLayerName = "VK_LAYER_KHRONOS_validation";

	bool validationLayerFound = false;
	for (u32 i = 0; i < propertyCount; i++)
	{
		if (strcmp(validationLayerName, layerProperties[i].layerName) == 0)
		{
			validationLayerFound = true;
			break;
		}
	}

	assert(validationLayerFound && "Validation layer not found!");
	char *enabledLayers[] = { validationLayerName };

	char *extensions[] =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};
	
	VkApplicationInfo appInfo =
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		NULL,
		"My Clever App Name",
		1,
		"My Even Cleverer Engine Name",
		1,
		VK_API_VERSION_1_3
	};

	VkInstanceCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		NULL,
		0,
		&appInfo,
		array_count(enabledLayers),
		enabledLayers,
		array_count(extensions),
		extensions
	};

	if (vkCreateInstance(&createInfo, NULL,
						 &vk.instance) != VK_SUCCESS)
	{
		assert(!"Failed to create vulkan instance");
	}

	VkDebugUtilsMessageSeverityFlagsEXT messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	VkDebugUtilsMessageTypeFlagsEXT messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo =
	{
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		NULL,
		0,
		messageSeverity,
		messageType,
		vulkan_debug_callback,
		NULL
	};

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT =
	(PFN_vkCreateDebugUtilsMessengerEXT)
		vkGetInstanceProcAddr(vk.instance, "vkCreateDebugUtilsMessengerEXT");

	VkDebugUtilsMessengerEXT debugMessenger;
	if (vkCreateDebugUtilsMessengerEXT(vk.instance, &debugCreateInfo, NULL,
									   &debugMessenger) != VK_SUCCESS)
	{
		assert(!"Failed to create debug messenger!");
	}
	
	VkXlibSurfaceCreateInfoKHR surfaceCreateInfo =
	{
		VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
		NULL,
		0,
		vk.display,
		vk.window
	};

	if (vkCreateXlibSurfaceKHR(vk.instance, &surfaceCreateInfo, NULL,
							   &vk.surface) != VK_SUCCESS)
	{
		assert(!"Failed to create surface");
	}
	
	u32 deviceCount = 0;
	vkEnumeratePhysicalDevices(vk.instance, &deviceCount, NULL);
	assert(deviceCount <= 8);
	
	VkPhysicalDevice devices[8] = {NULL};
	vkEnumeratePhysicalDevices(vk.instance, &deviceCount, devices);
	vk.physicalDevice = devices[0];
	
	for (u32 i = 0; i < deviceCount; i++)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(devices[i], &props);
		
		if (props.deviceType ==
		VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			vk.physicalDevice = devices[i];
			break;
		}
	}
	assert(vk.physicalDevice);
	
	u32 queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vk.physicalDevice,
	&queueFamilyPropertyCount, NULL);
	
	assert(queueFamilyPropertyCount <= 3);
	VkQueueFamilyProperties queueFamilyProperties[3] = {0};
	vkGetPhysicalDeviceQueueFamilyProperties(vk.physicalDevice,
	&queueFamilyPropertyCount,
	queueFamilyProperties);
	u32 queueFamilyIndex = 0;
	
	assert(queueFamilyProperties[queueFamilyIndex].queueFlags
	& VK_QUEUE_GRAPHICS_BIT);
	
	VkBool32 presentSupport = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(vk.physicalDevice, queueFamilyIndex,
	vk.surface,
	&presentSupport);
	
	assert(presentSupport);
	
	vk.graphicsAndPresentQueueFamily = queueFamilyIndex;
	
	F32 queuePriorities[] = { 1.0f };
	VkDeviceQueueCreateInfo queueCreateInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		NULL,
		0,
		vk.graphicsAndPresentQueueFamily,
		array_count(queuePriorities),
		queuePriorities
	};

	VkDeviceQueueCreateInfo queueCreateInfos[] = {queueCreateInfo};

	char *deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	VkDeviceCreateInfo deviceCreateInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		NULL,
		0,
		array_count(queueCreateInfos),
		queueCreateInfos,
		0,
		NULL,
		array_count(deviceExtensions),
		deviceExtensions,
		NULL
	};

	if (vkCreateDevice(vk.physicalDevice, &deviceCreateInfo, NULL,
					   &vk.device) != VK_SUCCESS)
	{
		assert(!"Failed to create logical device");
	}
	
	vkGetDeviceQueue(vk.device, vk.graphicsAndPresentQueueFamily, 0,
	&vk.graphicsAndPresentQueue);
	
	assert(vk.graphicsAndPresentQueue);
	
	
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk.physicalDevice, vk.surface,
											  &surfaceCapabilities);

	vk.swapchainImageFormat = VK_FORMAT_B8G8R8A8_SRGB;
	vk.swapchainExtents = surfaceCapabilities.currentExtent;

	VkSwapchainCreateInfoKHR swapchainCreateInfo =
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		NULL,
		0,
		vk.surface,
		array_count(vk.swapchainImages),
		vk.swapchainImageFormat,
		VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		vk.swapchainExtents,
		1,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		NULL,
		surfaceCapabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_PRESENT_MODE_FIFO_KHR,
		VK_TRUE,
		NULL
	};

	if (vkCreateSwapchainKHR(vk.device, &swapchainCreateInfo, NULL,
							 &vk.swapchain) != VK_SUCCESS)
	{
		assert(!"Failed to create the swapchain");
	}
	
	u32 imageCount = 0;
	vkGetSwapchainImagesKHR(vk.device, vk.swapchain, &imageCount, NULL);
	assert(imageCount == array_count(vk.swapchainImages));

	vkGetSwapchainImagesKHR(vk.device, vk.swapchain, &imageCount,
							vk.swapchainImages);

	for (u32 i = 0; i < imageCount; i++)
	{
		assert(vk.swapchainImages[i]);
		
		VkComponentMapping swizzle =
		{
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY
		};
		
		VkImageSubresourceRange subRange =
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1
		};
		
		VkImageViewCreateInfo viewInfo =
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			NULL,
			0,
			vk.swapchainImages[i],
			VK_IMAGE_VIEW_TYPE_2D,
			vk.swapchainImageFormat,
			swizzle,
			subRange
		};
		
		vkCreateImageView(vk.device, &viewInfo, NULL,
						  &vk.swapchainImageViews[i]);
		
		assert(vk.swapchainImageViews[i]);
	}

	return vk;
}

VkShaderModule
create_shader_module(VulkanContext *vk, void *code, size_t size)
{
	VkShaderModule result;
	        
	VkShaderModuleCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		NULL,
		0,
		size,
		(u32 *)code
	};
		    
	if (vkCreateShaderModule(
			vk->device,
			&createInfo,
			NULL,
			&result
		) != VK_SUCCESS
	) {
		assert(!"Failed to create shader module!");
	}
	
	return result;
}

int main(int argc, char** argv) {
	x11_init_vulkan(0,0,144,144,"a");	
	return 0;
}

