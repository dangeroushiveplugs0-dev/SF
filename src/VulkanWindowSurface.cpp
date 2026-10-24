#include "VulkanWindowSurface.hpp"

#include <vector>

VulkanWindowSurface::~VulkanWindowSurface() {
    shutdown();
}

bool VulkanWindowSurface::initialize(
    std::string& deviceName,
    uint32_t& graphicsQueueFamilyIndex,
    std::string& errorMessage
) {
    if (m_instance != VK_NULL_HANDLE || m_window != nullptr) {
        errorMessage = "Vulkan window surface is already initialized";
        return false;
    }

    if (glfwInit() != GLFW_TRUE) {
        errorMessage = "glfwInit failed (a display server may be unavailable)";
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    m_window = glfwCreateWindow(320, 240, "SF Vulkan Surface Probe", nullptr, nullptr);
    if (m_window == nullptr) {
        errorMessage = "GLFW could not create a Vulkan-compatible window";
        shutdown();
        return false;
    }

    uint32_t extensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (glfwExtensions == nullptr || extensionCount == 0) {
        errorMessage = "GLFW did not provide required Vulkan instance extensions";
        shutdown();
        return false;
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "SF";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "SF Rendering";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = extensionCount;
    instanceInfo.ppEnabledExtensionNames = glfwExtensions;

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS) {
        errorMessage = "vkCreateInstance for window surface failed with VkResult " +
            std::to_string(static_cast<int>(result));
        shutdown();
        return false;
    }

    result = glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface);
    if (result != VK_SUCCESS) {
        errorMessage = "glfwCreateWindowSurface failed with VkResult " +
            std::to_string(static_cast<int>(result));
        shutdown();
        return false;
    }

    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (result != VK_SUCCESS || deviceCount == 0) {
        errorMessage = "No Vulkan physical devices available for presentation";
        shutdown();
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
    if (result != VK_SUCCESS) {
        errorMessage = "Vulkan physical-device enumeration failed";
        shutdown();
        return false;
    }

    for (VkPhysicalDevice candidate : devices) {
        uint32_t familyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(candidate, &familyCount, nullptr);
        std::vector<VkQueueFamilyProperties> families(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(candidate, &familyCount, families.data());

        for (uint32_t index = 0; index < familyCount; ++index) {
            if (families[index].queueCount == 0 ||
                (families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
                continue;
            }

            VkBool32 canPresent = VK_FALSE;
            result = vkGetPhysicalDeviceSurfaceSupportKHR(
                candidate, index, m_surface, &canPresent
            );
            if (result == VK_SUCCESS && canPresent == VK_TRUE) {
                m_physicalDevice = candidate;
                m_graphicsQueueFamilyIndex = index;
                VkPhysicalDeviceProperties properties{};
                vkGetPhysicalDeviceProperties(candidate, &properties);
                deviceName = properties.deviceName;
                graphicsQueueFamilyIndex = index;
                return true;
            }
        }
    }

    errorMessage = "No Vulkan queue family supports both graphics and this window surface";
    shutdown();
    return false;
}

void VulkanWindowSurface::shutdown() {
    if (m_surface != VK_NULL_HANDLE && m_instance != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
    m_physicalDevice = VK_NULL_HANDLE;
    m_graphicsQueueFamilyIndex = UINT32_MAX;

    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
    if (m_window != nullptr) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();
    }
}
