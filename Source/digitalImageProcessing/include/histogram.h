#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

// ====================================================================
//  作业2-1：迭代直方图均衡化
// ====================================================================
//  算术约定：uint8_t 灰度级 [0, 255]，CDF 归一化后线性映射回 [0, 255]

/**
 * @brief  从直方图计算累积分布函数 CDF（已实现，无需修改）
 */
inline std::vector<float> computeCDF(const std::vector<size_t>& hist, size_t total_pixels) {
    std::vector<float> cdf(256, 0.0f);
    if (total_pixels == 0) return cdf;
    float sum = 0.0f;
    for (int i = 0; i < 256; ++i) { sum += hist[i]; cdf[i] = sum / total_pixels; }
    return cdf;
}

/**
 * @brief  逐像素比较两张图像是否完全相同（已实现，无需修改）
 */
template<std::size_t N, typename T = uint8_t>
bool imageEqual(const Image<N, T>& a, const Image<N, T>& b) {
    if (a.rows() != b.rows() || a.cols() != b.cols()) return false;
    for (size_t i = 0; i < a.rows(); ++i)
        for (size_t j = 0; j < a.cols(); ++j)
            for (std::size_t c = 0; c < N; ++c)
                if (a.at(i, j)[c] != b.at(i, j)[c]) return false;
    return true;
}

// ====================================================================
//  TODO：以下三个函数需要实现
// ====================================================================

/**
 * @brief  单次直方图均衡化（TODO）
 * @tparam N  通道数
 * @tparam T  像素类型（仅支持 uint8_t）
 * @param  image  输入图像
 * @return 均衡化后的图像
 *
 * 算法步骤：
 *   1. 统计每个通道的 256 级直方图
 *   2. 利用 computeCDF() 计算映射表 mappings[c][i] = round(cdf[i] * 255)
 *   3. 应用映射表生成新图像
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> histogramEqualization(const Image<N, T>& image) {
    static_assert(std::is_same_v<T, uint8_t>);
    return Image<N, T>(0, 0);  // TODO: 替换为你的实现
}

/**
 * @brief  连续 n 次直方图均衡化（TODO）
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> histogramEqualizationN(const Image<N, T>& image, int n) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;  // TODO: 替换为你的实现
}

/**
 * @brief  迭代直方图均衡化至收敛（TODO）
 * @param  max_iterations  最大迭代次数（-1 = 无上限）
 * @return pair<最终图像, 实际迭代次数>
 *
 * 使用 imageEqual() 判断收敛。
 */
template<std::size_t N, typename T = uint8_t>
std::pair<Image<N, T>, int> histogramEqualizationIterative(
    const Image<N, T>& image, int max_iterations = -1) {
    static_assert(std::is_same_v<T, uint8_t>);
    return {image, 0};  // TODO: 替换为你的实现
}

/**
 * @brief  调试用直方图打印（已实现，无需修改）
 */
template<typename T = uint8_t>
void printHistogram(const std::vector<size_t>& hist, int max_width = 80) {
    size_t max_count = *std::max_element(hist.begin(), hist.end());
    if (max_count == 0) return;
    for (int i = 0; i < 256; i += 16) {
        std::cout << "Values " << i << "-" << std::min(i + 15, 255) << ": ";
        int bar_width = static_cast<int>(static_cast<float>(hist[i]) / max_count * max_width);
        std::cout << std::string(bar_width, '#') << " (" << hist[i] << ")" << std::endl;
    }
}
