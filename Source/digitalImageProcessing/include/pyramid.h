#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <vector>

// ====================================================================
//  作业3-1：近似金字塔与预测残差金字塔
// ====================================================================
//  参考教材例7.1。高斯核 w=[1,4,6,4,1]/16。

/**
 * @brief  5×5 高斯核 1D 分量（已实现，无需修改）
 */
inline std::vector<float> gaussianKernel1D() {
    return {1.0f / 16.0f, 4.0f / 16.0f, 6.0f / 16.0f, 4.0f / 16.0f, 1.0f / 16.0f};
}

/**
 * @brief  2× 降采样（已实现，无需修改）
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> downsample2(const Image<N, T>& image) {
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows / 2, cols / 2);
    for (int i = 0; i < rows / 2; ++i)
        for (int j = 0; j < cols / 2; ++j) result(i, j) = image.at(i * 2, j * 2);
    return result;
}

/**
 * @brief  2× 上采样（已实现，无需修改）
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> upsample2(const Image<N, T>& image) {
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows * 2, cols * 2);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j) {
            auto p = image.at(i, j);
            result(i * 2, j * 2) = p; result(i * 2, j * 2 + 1) = p;
            result(i * 2 + 1, j * 2) = p; result(i * 2 + 1, j * 2 + 1) = p;
        }
    return result;
}

// ====================================================================
//  TODO：以下三个函数需要实现
// ====================================================================

/**
 * @brief  5×5 可分离高斯滤波（TODO）
 *
 * 先水平方向一维卷积（Image<N,float> 中间结果），再垂直方向。
 * 使用 gaussianKernel1D() 获取权重，边界 at_cut() clamp。
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> gaussianBlur(const Image<N, T>& image) {
    return image;  // TODO: 替换为你的实现
}

/**
 * @brief  近似金字塔 = 逐级高斯模糊 + 降采样（TODO）
 * @return 各级图像 vector（第0级=原图）
 */
template<std::size_t N, typename T = uint8_t>
std::vector<Image<N, T>> buildApproximationPyramid(const Image<N, T>& image, int levels) {
    return {image};  // TODO: 替换为你的实现
}

/**
 * @brief  预测残差金字塔 = 近似金字塔各级之差 + 128（TODO）
 */
template<std::size_t N, typename T = uint8_t>
std::vector<Image<N, T>> buildResidualPyramid(const Image<N, T>& image, int levels) {
    return {image};  // TODO: 替换为你的实现
}
