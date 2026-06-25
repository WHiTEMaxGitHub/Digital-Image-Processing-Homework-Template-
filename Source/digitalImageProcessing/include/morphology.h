#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>

// ====================================================================
//  作业4：形态学处理（二值 + 灰度）
// ====================================================================
//  标 "已实现" 的是工具/组合函数，可直接使用。
//  标 "TODO" 的是你需要实现的核心算法（共 10 个）。

// ==================== 结构元素（已实现） ====================
struct StructuringElement { int rows, cols, origin_r, origin_c; std::vector<std::vector<bool>> data; };
inline StructuringElement rectSE(int rows, int cols) { StructuringElement se{rows, cols, rows/2, cols/2, {}}; se.data.assign(rows, std::vector<bool>(cols, true)); return se; }
inline StructuringElement hlineSE(int length) { StructuringElement se{1, length, 0, length/2, {}}; se.data.assign(1, std::vector<bool>(length, true)); return se; }
inline StructuringElement vlineSE(int length) { StructuringElement se{length, 1, length/2, 0, {}}; se.data.assign(length, std::vector<bool>(1, true)); return se; }
inline StructuringElement crossSE(int size) { StructuringElement se{size, size, size/2, size/2, {}}; se.data.assign(size, std::vector<bool>(size, false)); int c = size/2; for (int i = 0; i < size; ++i) { se.data[i][c] = true; se.data[c][i] = true; } return se; }
inline StructuringElement diskSE(int radius) { int size = 2*radius+1; StructuringElement se{size, size, radius, radius, {}}; se.data.assign(size, std::vector<bool>(size, false)); int r2 = radius*radius; for (int i = -radius; i <= radius; ++i) for (int j = -radius; j <= radius; ++j) if (i*i+j*j <= r2) se.data[i+radius][j+radius] = true; return se; }

// ==================== 辅助函数（已实现） ====================
template<std::size_t N, typename T = uint8_t>
Image<N, T> threshold(const Image<N, T>& image, uint8_t th = 128) {
    static_assert(std::is_same_v<T, uint8_t>);
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c) for (int i = 0; i < rows; ++i) for (int j = 0; j < cols; ++j)
        result(i, j)[c] = (image.at(i, j)[c] >= th) ? 255 : 0;
    return result;
}
template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryNot(const Image<N, T>& image) {
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c) for (int i = 0; i < rows; ++i) for (int j = 0; j < cols; ++j)
        result(i, j)[c] = (image.at(i, j)[c] == 255) ? 0 : 255;
    return result;
}
template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryAnd(const Image<N, T>& a, const Image<N, T>& b) {
    int rows = static_cast<int>(a.rows()), cols = static_cast<int>(a.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c) for (int i = 0; i < rows; ++i) for (int j = 0; j < cols; ++j)
        result(i, j)[c] = (a.at(i, j)[c] == 255 && b.at(i, j)[c] == 255) ? 255 : 0;
    return result;
}

// ==================== TODO: 二值形态学核心（5 个） ====================

/**
 * @brief  二值膨胀（TODO） —— SE 覆盖区域内存在任一前景像素则输出前景
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryDilate(const Image<N, T>& image, const StructuringElement& se) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;  // TODO
}
/**
 * @brief  二值腐蚀（TODO） —— SE 覆盖区域内所有像素均为前景才输出前景
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryErode(const Image<N, T>& image, const StructuringElement& se) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;  // TODO
}

// ==================== 二值组合操作（已实现） ====================
template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryOpen(const Image<N, T>& image, const StructuringElement& se) { return binaryDilate(binaryErode(image, se), se); }
template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryClose(const Image<N, T>& image, const StructuringElement& se) { return binaryErode(binaryDilate(image, se), se); }
template<std::size_t N, typename T = uint8_t>
Image<N, T> geodesicDilate(const Image<N, T>& marker, const Image<N, T>& mask, const StructuringElement& se) { return binaryAnd(binaryDilate(marker, se), mask); }

/**
 * @brief  形态学重构（TODO） —— 迭代测地膨胀直至稳定
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> morphReconstruct(const Image<N, T>& marker, const Image<N, T>& mask, const StructuringElement& se, int max_iter = -1) {
    return marker;  // TODO
}

/** @brief  长字符提取（已实现） */
template<std::size_t N, typename T = uint8_t>
Image<N, T> longCharacterExtraction(const Image<N, T>& binary, int length = 51) { return binaryOpen(binary, hlineSE(length)); }

/**
 * @brief  空洞填充（TODO，参考图9.31）
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> holeFilling(const Image<N, T>& binary) { return binary; }  // TODO
/**
 * @brief  边界清除（TODO，参考图9.32）
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> borderClearing(const Image<N, T>& binary) { return binary; }  // TODO

// ==================== TODO: 灰度形态学核心（2 个） ====================

/**
 * @brief  灰度膨胀（TODO） —— 取局部最大值
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> grayDilate(const Image<N, T>& image, const StructuringElement& se) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;  // TODO
}
/**
 * @brief  灰度腐蚀（TODO） —— 取局部最小值
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> grayErode(const Image<N, T>& image, const StructuringElement& se) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;  // TODO
}

// ==================== 灰度组合操作（已实现） ====================
template<std::size_t N, typename T = uint8_t>
Image<N, T> grayOpen(const Image<N, T>& image, const StructuringElement& se) { return grayDilate(grayErode(image, se), se); }
template<std::size_t N, typename T = uint8_t>
Image<N, T> grayClose(const Image<N, T>& image, const StructuringElement& se) { return grayErode(grayDilate(image, se), se); }
template<std::size_t N, typename T = uint8_t>
Image<N, T> topHatTransform(const Image<N, T>& image, const StructuringElement& se) {
    auto opened = grayOpen(image, se);
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c) for (int i = 0; i < rows; ++i) for (int j = 0; j < cols; ++j)
        result(i, j)[c] = static_cast<T>(std::clamp(static_cast<int>(image.at(i, j)[c]) - opened.at(i, j)[c], 0, 255));
    return result;
}
template<std::size_t N, typename T = uint8_t>
Image<N, T> topHatCorrect(const Image<N, T>& image, int se_size = 40) { return topHatTransform(image, diskSE(se_size)); }
template<std::size_t N, typename T = uint8_t>
Image<N, T> morphGradient(const Image<N, T>& image, const StructuringElement& se) {
    auto dilated = grayDilate(image, se), eroded = grayErode(image, se);
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c) for (int i = 0; i < rows; ++i) for (int j = 0; j < cols; ++j)
        result(i, j)[c] = static_cast<T>(std::clamp(static_cast<int>(dilated.at(i, j)[c]) - eroded.at(i, j)[c], 0, 255));
    return result;
}
template<std::size_t N, typename T = uint8_t>
Image<N, T> watershedPreprocess(const Image<N, T>& image, int smooth_radius = 3, int grad_radius = 3) {
    auto se_smooth = diskSE(smooth_radius);
    auto smoothed = grayClose(grayOpen(image, se_smooth), se_smooth);
    return morphGradient(smoothed, diskSE(grad_radius));
}

// ==================== TODO: 粒度测定与分水岭标记（3 个） ====================

/**
 * @brief  粒度测定（TODO，参考图9.41）
 */
template<std::size_t N, typename T = uint8_t>
std::vector<int> granulometry(const Image<N, T>& binary, int max_radius = 30) { return {}; }  // TODO
/**
 * @brief  粒度测定可视化（TODO）
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> granulometryVisualize(const Image<N, T>& binary, int max_radius = 30) { return binary; }  // TODO
/**
 * @brief  分水岭标记（TODO，Otsu 阈值 + 形态学平滑）
 */
template<std::size_t N, typename T = uint8_t>
Image<N, T> watershedMarkers(const Image<N, T>& image, int min_distance = 10) { return image; }  // TODO
