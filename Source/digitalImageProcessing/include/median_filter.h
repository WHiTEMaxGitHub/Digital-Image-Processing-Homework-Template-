#pragma once
#include "image.h"
#include "histogram.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>

// ====================================================================
//  作业2-2：迭代中值滤波
// ====================================================================
//  滤波核大小为 3×3（奇数），使用 std::nth_element 取中值。

/**
 * @brief  单次中值滤波（TODO）
 * @param  kernel_size  滤波核大小（奇数，本题使用 3）
 *
 * 算法：遍历所有像素，收集邻域值到 buffer，std::nth_element 取中值。
 * 边界使用 image.at_cut() clamp。
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> medianFilter(const Image<N, T>& image, int kernel_size = 3) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;  // TODO: 替换为你的实现
}

/**
 * @brief  连续 n 次中值滤波（TODO）
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> medianFilterN(const Image<N, T>& image, int n, int kernel_size = 3) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;  // TODO: 替换为你的实现
}

/**
 * @brief  迭代中值滤波至收敛（TODO）
 * @return pair<最终图像, 实际迭代次数>
 *
 * 使用 imageEqual()（来自 histogram.h）判断收敛。
 */
template<std::size_t N, typename T = uint8_t>
std::pair<Image<N, T>, int> medianFilterIterative(
    const Image<N, T>& image, int max_iterations = -1, int kernel_size = 3) {
    static_assert(std::is_same_v<T, uint8_t>);
    return {image, 0};  // TODO: 替换为你的实现
}
