#pragma once

#include <cstdint>
#include <string>
#include <vulkan/vulkan.h>

// Headless Vulkan bootstrap kept independent from the active OpenGL renderer.
// Discovers a physical device and a queue family capable of graphics commands.
class VulkanRuntime {
public:
    VulkanRuntime() = default;
    ~VulkanRuntime();

    VulkanRuntime(const VulkanRuntime&) = delete;
    VulkanRuntime& operator=(const VulkanRuntime&) = delete;

    bool initializeHeadless(
        std::string& deviceName,
        uint32_t& graphicsQueueFamilyIndex,
        std::string& errorMessage
    );
    void shutdown();

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    uint32_t m_graphicsQueueFamilyIndex = UINT32_MAX;
};
