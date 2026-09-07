bool RukkySelectRenderDevice(VkDevice* device) {
    VkPhysicalDevice physicalDevice;

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

    int graphicsFamilyIndex = -1;
    for (int i = 0; i < queueFamilies.size(); i++) {
	if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
	    graphicsFamilyIndex = i;
	    break;
	}
    }

    f32 queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;


}
