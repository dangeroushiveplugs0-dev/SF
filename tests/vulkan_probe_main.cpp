#include "VulkanRuntime.hpp"

#include <iostream>
#include <string>

int main() {
    VulkanRuntime runtime;
    std::string deviceName;
    std::string errorMessage;
    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;

    if (!runtime.initializeHeadless(
            deviceName, graphicsQueueFamilyIndex, errorMessage)) {
        std::cerr << "Vulkan smoke test failed: " << errorMessage << '\n';
        return 1;
    }

    if (runtime.device() == VK_NULL_HANDLE ||
        runtime.graphicsQueue() == VK_NULL_HANDLE ||
        runtime.physicalDevice() == VK_NULL_HANDLE) {
        std::cerr << "Vulkan smoke test failed: expected device/queue handles are null\n";
        return 1;
    }

    std::cout << "Vulkan instance initialized successfully.\n";
    std::cout << "Physical device detected: " << deviceName << '\n';
    std::cout << "Graphics queue family selected: "
              << graphicsQueueFamilyIndex << '\n';
    std::cout << "Logical device and graphics queue created successfully.\n";
    return 0;
}
