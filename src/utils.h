#pragma once

#include <cassert>
#include <functional>
#include <iostream>

#define VK_CHECK(x)                                                                                                                        \
    do {                                                                                                                                   \
        VkResult err = x;                                                                                                                  \
        if (err) {                                                                                                                         \
            std::cerr << __FILE__ << ":" << __LINE__ << " -> VULKAN ERROR: " << err << std::endl;                                          \
            abort();                                                                                                                       \
        }                                                                                                                                  \
    } while (0)

// from: https://stackoverflow.com/a/57595105
template<typename T, typename... Rest>
void hashCombine(std::size_t &seed, const T &v, const Rest &...rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
};
