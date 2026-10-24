#include "VulkanRuntime.hpp"

#include <vector>

VulkanRuntime::~VulkanRuntime() {
    shutdown();
}

bool VulkanRuntime::initializeHeadless(
    std::string& deviceName,
    uint32_t& graphicsQueueFamilyIndex,
    std::string& errorMessage
) {
    if (m_instance != VK_NULL_HANDLE) {
        errorMessage = "Vulkan runtime is already initialized";
        return false;
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "SF";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "SF Rendering";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS) {
        errorMessage = "vkCreateInstance failed with VkResult " +
            std::to_string(static_cast<int>(result));
        m_instance = VK_NULL_HANDLE;
        return false;
    }

    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (result != VK_SUCCESS) {
        errorMessage = "vkEnumeratePhysicalDevices(count) failed with VkResult " +
            std::to_string(static_cast<int>(result));
        shutdown();
        return false;
    }

    if (deviceCount == 0) {
        errorMessage = "Vulkan instance created, but no physical devices were found";
        shutdown();
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
    if (result != VK_SUCCESS || devices.empty()) {
        errorMessage = "Vulkan physical-device enumeration failed with VkResult " +
            std::to_string(static_cast<int>(result));
        shutdown();
        return false;
    }

    for (VkPhysicalDevice candidate : devices) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(candidate, &queueFamilyCount, nullptr);
        if (queueFamilyCount == 0) {
            continue;
        }

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            candidate, &queueFamilyCount, queueFamilies.data()
        );

        for (uint32_t index = 0; index < queueFamilyCount; ++index) {
            const VkQueueFamilyProperties& family = queueFamilies[index];
            if (family.queueCount == 0 ||
                (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
                continue;
            }

            m_physicalDevice = candidate;
            m_graphicsQueueFamilyIndex = index;

            const float queuePriority = 1.0f;
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = index;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;

            VkDeviceCreateInfo deviceInfo{};
            deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceInfo.queueCreateInfoCount = 1;
            deviceInfo.pQueueCreateInfos = &queueInfo;

            result = vkCreateDevice(m_physicalDevice, &deviceInfo, nullptr, &m_device);
            if (result != VK_SUCCESS) {
                errorMessage = "vkCreateDevice failed with VkResult " +
                    std::to_string(static_cast<int>(result));
                shutdown();
                return false;
            }

            vkGetDeviceQueue(m_device, index, 0, &m_graphicsQueue);
            if (m_graphicsQueue == VK_NULL_HANDLE) {
                errorMessage = "Vulkan logical device was created, but its graphics queue is null";
                shutdown();
                return false;
            }

            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
            deviceName = properties.deviceName;
            graphicsQueueFamilyIndex = index;
            return true;
        }
    }

    errorMessage = "Vulkan devices were found, but none exposes a graphics-capable queue family";
    shutdown();
    return false;
}

void VulkanRuntime::shutdown() {
    if (m_device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_device);
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }
    m_graphicsQueue = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;
    m_graphicsQueueFamilyIndex = UINT32_MAX;

    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}
