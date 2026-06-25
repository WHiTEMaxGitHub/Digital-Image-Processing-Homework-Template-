#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cmath>

enum class LaplacianType { FOUR_NEIGHBOR, EIGHT_NEIGHBOR };

template<std::size_t N, typename T = uint8_t>
Image<N, T> laplacianSharpen(const Image<N, T>& image,
                              LaplacianType type = LaplacianType::FOUR_NEIGHBOR,
                              float strength = 1.0f) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> laplacianSharpen4(const Image<N, T>& image, float strength = 1.0f) {
    return laplacianSharpen(image, LaplacianType::FOUR_NEIGHBOR, strength);
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> laplacianSharpen8(const Image<N, T>& image, float strength = 1.0f) {
    return laplacianSharpen(image, LaplacianType::EIGHT_NEIGHBOR, strength);
}
