#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <algorithm>

// ====================================================================
//  作业1：图像插值
// ====================================================================
//  请实现以下三个插值函数。每个函数接受原图指针和行列缩放倍数，
//  返回缩放后的图像。
//
//  可用 API：
//    - origin->getSize()                → std::pair<size_t, size_t> (rows, cols)
//    - origin->at_cut(x, y)            → Pixel<N,T>（边界裁剪访问）
//    - Image<N, T>(rows, cols)         → 构造指定尺寸的图像
//    - processed(x, y) = pixel         → 写入像素
// ====================================================================

// -------------------------------------------------------------------
//  cubic_weight — 双三次插值权重函数（已实现，无需修改）
// -------------------------------------------------------------------
/**
 * @brief  双三次插值的权重函数（Mitchell-Netravali, B=C=0 特例 a=-0.5）
 * @param  t  到目标像素的距离（已取绝对值）
 * @return 权重值
 */
inline float cubic_weight(float t) {
    t = std::abs(t);
    float a = -0.5f;
    if (t < 1.0f) return ((a + 2) * t - (a + 3)) * t * t + 1;
    if (t < 2.0f) return ((a * t - 5 * a) * t + 8 * a) * t - 4 * a;
    return 0.0f;
}

// ====================================================================
//  TODO：以下三个函数需要实现
// ====================================================================

// -------------------------------------------------------------------
//  nearestNeighborInterpolation — 最近邻插值（TODO）
// -------------------------------------------------------------------
/**
 * @brief  最近邻插值
 * @tparam N  通道数
 * @tparam T  像素类型（默认 uint8_t）
 * @param  origin     原图指针
 * @param  row_zoom   行方向缩放倍数（>1 放大，<1 缩小）
 * @param  col_zoom   列方向缩放倍数（>1 放大，<1 缩小）
 * @return 缩放后的图像
 *
 * 算法步骤：
 *   1. 获取原图尺寸：auto [originalRows, originalCols] = origin->getSize();
 *   2. 计算新图尺寸：
 *        int newRows = static_cast<int>(originalRows * row_zoom);
 *        int newCols = static_cast<int>(originalCols * col_zoom);
 *   3. 构造目标图像 Image<N, T> processed(newRows, newCols);
 *   4. 遍历每个目标像素 (x, y)：
 *        float originalX = x / row_zoom;
 *        float originalY = y / col_zoom;
 *        int nearestX = static_cast<int>(originalX + 0.5f);   // 四舍五入
 *        int nearestY = static_cast<int>(originalY + 0.5f);
 *        processed(x, y) = origin->at_cut(nearestX, nearestY);
 *   5. return processed;
 */
template <std::size_t N, typename T = uint8_t>
Image<N, T> nearestNeighborInterpolation(Image<N, T>* origin, float row_zoom, float col_zoom) {
    auto [originalRows, originalCols] = origin->getSize();
    return Image<N, T>(0, 0);  // TODO: 替换为你的实现
}

// -------------------------------------------------------------------
//  bi_LinearInterpolation — 双线性插值（TODO）
// -------------------------------------------------------------------
/**
 * @brief  双线性插值
 * @tparam N  通道数
 * @tparam T  像素类型（默认 uint8_t）
 * @param  origin     原图指针
 * @param  row_zoom   行方向缩放倍数
 * @param  col_zoom   列方向缩放倍数
 * @return 缩放后的图像
 *
 * 算法步骤：
 *   1. 获取原图尺寸，计算新图尺寸（同上）
 *   2. 计算缩放因子：
 *        float scaleX = (float)originalCols / newCols;
 *        float scaleY = (float)originalRows / newRows;
 *   3. 遍历每个目标像素 (x, y)：
 *        a) 像素中心对齐映射：
 *             srcX = (x + 0.5f) * scaleY - 0.5f;
 *             srcY = (y + 0.5f) * scaleX - 0.5f;
 *        b) 确定四个邻域：
 *             int x0 = (int)srcX, y0 = (int)srcY;
 *             int x1 = x0 + 1, y1 = y0 + 1;
 *             float dx = srcX - x0, dy = srcY - y0;  // 小数部分
 *        c) 读取四个邻域像素：v00, v01, v10, v11
 *        d) 对每个通道双线性插值：
 *             top    = v00*(1-dy) + v01*dy
 *             bottom = v10*(1-dy) + v11*dy
 *             val    = top*(1-dx) + bottom*dx
 *        e) clamp 到 [0, 255]，四舍五入回 T 类型
 *   4. return processed;
 */
template <std::size_t N, typename T = uint8_t>
Image<N, T> bi_LinearInterpolation(Image<N, T>* origin, float row_zoom, float col_zoom) {
    auto [originalRows, originalCols] = origin->getSize();
    return Image<N, T>(0, 0);  // TODO: 替换为你的实现
}

// -------------------------------------------------------------------
//  bi_CubicInterpolation — 双三次插值（TODO）
// -------------------------------------------------------------------
/**
 * @brief  双三次插值（16 邻域加权）
 * @tparam N  通道数
 * @tparam T  像素类型
 * @param  origin     原图指针
 * @param  row_zoom   行方向缩放倍数
 * @param  col_zoom   列方向缩放倍数
 * @return 缩放后的图像
 *
 * 算法步骤：
 *   1. 获取原图尺寸，计算新图尺寸（同上）
 *   2. 遍历每个目标像素 (x, y)：
 *        a) 像素中心对齐映射：
 *             srcX = (x + 0.5f) * scaleY - 0.5f;
 *             srcY = (y + 0.5f) * scaleX - 0.5f;
 *        b) 基准点和偏移：
 *             int x0 = (int)floor(srcX), y0 = (int)floor(srcY);
 *             float dx = srcX - x0, dy = srcY - y0;
 *        c) 遍历 4×4=16 邻域 (i, j ∈ {-1, 0, 1, 2})：
 *             float weight_x = cubic_weight(dx - i);
 *             float weight_y = cubic_weight(dy - j);
 *             float weight   = weight_x * weight_y;
 *             sum += pixel_value * weight;  // 每个通道独立累加
 *        d) clamp 到 [0, 255]，四舍五入
 *   3. return processed;
 *
 * @note  使用上面定义的 cubic_weight() 函数计算每个邻域的权重
 */
template <std::size_t N, typename T>
Image<N, T> bi_CubicInterpolation(Image<N, T>* origin, float row_zoom, float col_zoom) {
    auto [originalRows, originalCols] = origin->getSize();
    return Image<N, T>(0, 0);  // TODO: 替换为你的实现
}
