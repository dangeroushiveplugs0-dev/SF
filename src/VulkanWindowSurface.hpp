#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// Creates a hidden GLFW window and a Vulkan surface for presentation testing.
// This is a separate probe path; the active OpenGL renderer is not changed.
class VulkanWindowSurface {
public:
    VulkanWindowSurface() = default;
    ~VulkanWindowSurface();

    VulkanWindowSurface(const VulkanWindowSurface&) = delete;
    VulkanWindowSurface& operator=(const VulkanWindowSurface&) = delete;

    bool initialize(std::string& deviceName, uint32_t& graphicsQueueFamilyIndex,
                    std::string& errorMessage);
    void shutdown();

    VkInstance instance() const { return m_instance; }
    VkSurfaceKHR surface() const { return m_surface; }
    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
    uint32_t graphicsQueueFamilyIndex() const { return m_graphicsQueueFamilyIndex; }

private:
    GLFWwindow* m_window = nullptr;
    VkInstance m_instance = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    uint32_t m_graphicsQueueFamilyIndex = UINT32_MAX;
};
