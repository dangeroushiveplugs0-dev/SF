#include "VulkanWindowSurface.hpp"

#include <iostream>
#include <string>

int main() {
    VulkanWindowSurface surface;
    std::string deviceName;
    std::string errorMessage;
    uint32_t queueFamily = UINT32_MAX;

    if (!surface.initialize(deviceName, queueFamily, errorMessage)) {
        std::cerr << "Vulkan surface smoke test failed: " << errorMessage << '\n';
        return 1;
    }

    if (surface.instance() == VK_NULL_HANDLE ||
        surface.surface() == VK_NULL_HANDLE ||
        surface.physicalDevice() == VK_NULL_HANDLE) {
        std::cerr << "Vulkan surface smoke test failed: required handles are null\n";
        return 1;
    }

    std::cout << "Vulkan presentation surface created successfully.\n";
    std::cout << "Presentation-capable device: " << deviceName << '\n';
    std::cout << "Graphics/presentation queue family: " << queueFamily << '\n';
    return 0;
}
