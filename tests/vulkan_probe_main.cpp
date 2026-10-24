#include "VulkanRuntime.hpp"

#include <iostream>
#include <string>

int main() {
    VulkanRuntime runtime;
    std::string deviceName;
    std::string errorMessage;

    if (!runtime.initializeHeadless(deviceName, errorMessage)) {
        std::cerr << "Vulkan smoke test failed: " << errorMessage << '\n';
        return 1;
    }

    std::cout << "Vulkan instance initialized successfully.\n";
    std::cout << "Physical device detected: " << deviceName << '\n';
    return 0;
}
