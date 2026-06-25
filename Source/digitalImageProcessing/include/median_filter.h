#pragma once
#include "image.h"
#include "histogram.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>

template<std::size_t N, typename T = uint8_t>
Image<N, T> medianFilter(const Image<N, T>& image, int kernel_size = 3) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> medianFilterN(const Image<N, T>& image, int n, int kernel_size = 3) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;
}

template<std::size_t N, typename T = uint8_t>
std::pair<Image<N, T>, int> medianFilterIterative(
    const Image<N, T>& image, int max_iterations = -1, int kernel_size = 3) {
    static_assert(std::is_same_v<T, uint8_t>);
    return {image, 0};
}
