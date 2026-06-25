#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cmath>

// ====================================================================
//  作业2-3：拉普拉斯锐化
// ====================================================================

enum class LaplacianType {
    FOUR_NEIGHBOR,  // 4-邻域核: [0,-1,0; -1,4,-1; 0,-1,0]
    EIGHT_NEIGHBOR  // 8-邻域核: [-1,-1,-1; -1,8,-1; -1,-1,-1]
};

/**
 * @brief  拉普拉斯锐化（TODO）
 * @param  type      核类型（FOUR_NEIGHBOR / EIGHT_NEIGHBOR）
 * @param  strength  锐化强度（推荐 1.0）
 *
 * 公式：sharpened = center - strength * laplacian
 * 边界使用 image.at_cut()。
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> laplacianSharpen(const Image<N, T>& image,
                              LaplacianType type = LaplacianType::FOUR_NEIGHBOR,
                              float strength = 1.0f) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;  // TODO: 替换为你的实现
}

/**
 * @brief  4-邻域拉普拉斯锐化（已实现，无需修改）
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> laplacianSharpen4(const Image<N, T>& image, float strength = 1.0f) {
    return laplacianSharpen(image, LaplacianType::FOUR_NEIGHBOR, strength);
}

/**
 * @brief  8-邻域拉普拉斯锐化（已实现，无需修改）
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> laplacianSharpen8(const Image<N, T>& image, float strength = 1.0f) {
    return laplacianSharpen(image, LaplacianType::EIGHT_NEIGHBOR, strength);
}
