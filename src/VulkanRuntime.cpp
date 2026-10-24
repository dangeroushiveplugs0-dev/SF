#include "VulkanRuntime.hpp"

#include <vector>

VulkanRuntime::~VulkanRuntime() {
    shutdown();
}

bool VulkanRuntime::initializeHeadless(
    std::string& deviceName,
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

    const VkResult createResult = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (createResult != VK_SUCCESS) {
        errorMessage = "vkCreateInstance failed with VkResult " +
            std::to_string(static_cast<int>(createResult));
        m_instance = VK_NULL_HANDLE;
        return false;
    }

    uint32_t deviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
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

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(devices.front(), &properties);
    deviceName = properties.deviceName;
    return true;
}

void VulkanRuntime::shutdown() {
    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}
