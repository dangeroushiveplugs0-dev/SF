#pragma once

#include <string>
#include <vulkan/vulkan.h>

// Minimal Vulkan bootstrap kept independent from the active OpenGL renderer.
// This first stage validates instance creation and physical-device discovery.
class VulkanRuntime {
public:
    VulkanRuntime() = default;
    ~VulkanRuntime();

    VulkanRuntime(const VulkanRuntime&) = delete;
    VulkanRuntime& operator=(const VulkanRuntime&) = delete;

    bool initializeHeadless(std::string& deviceName, std::string& errorMessage);
    void shutdown();

private:
    VkInstance m_instance = VK_NULL_HANDLE;
};
