#pragma once

#include <cstdint>
#include <string>
#include <vulkan/vulkan.h>

// Headless Vulkan bootstrap kept independent from the active OpenGL renderer.
// Discovers a graphics queue and creates the logical device needed to use it.
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

    VkInstance instance() const { return m_instance; }
    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
    VkDevice device() const { return m_device; }
    VkQueue graphicsQueue() const { return m_graphicsQueue; }
    uint32_t graphicsQueueFamilyIndex() const { return m_graphicsQueueFamilyIndex; }

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    uint32_t m_graphicsQueueFamilyIndex = UINT32_MAX;
};
