#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <vector>

// ==================== 高斯核（5×5，可分离） ====================

inline std::vector<float> gaussianKernel1D() {
    // w = [1, 4, 6, 4, 1] / 16
    return {1.0f / 16.0f, 4.0f / 16.0f, 6.0f / 16.0f, 4.0f / 16.0f, 1.0f / 16.0f};
}

// 高斯滤波（使用 at_cut 边界 clamp）
template<std::size_t N, typename T = uint8_t>
Image<N, T> gaussianBlur(const Image<N, T>& image) {
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    auto kernel = gaussianKernel1D();
    int half = 2; // 5x5 kernel → half = 2

    // 先水平滤波
    Image<N, float> temp(rows, cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Pixel<N, float> sum;
            for (int k = -half; k <= half; ++k) {
                float w = kernel[k + half];
                auto p = image.at_cut(i, j + k);
                for (std::size_t c = 0; c < N; ++c)
                    sum[c] += w * static_cast<float>(p[c]);
            }
            temp(i, j) = sum;
        }
    }

    // 再垂直滤波
    Image<N, T> result(rows, cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Pixel<N, T> out;
            for (std::size_t c = 0; c < N; ++c) {
                float val = 0.0f;
                for (int k = -half; k <= half; ++k) {
                    float w = kernel[k + half];
                    val += w * temp.at_cut(i + k, j)[c];
                }
                if constexpr (std::is_same_v<T, uint8_t>)
                    out[c] = static_cast<T>(std::clamp(std::round(val), 0.0f, 255.0f));
                else
                    out[c] = static_cast<T>(val);
            }
            result(i, j) = out;
        }
    }

    return result;
}

// ==================== 降采样 2× ====================

template<std::size_t N, typename T = uint8_t>
Image<N, T> downsample2(const Image<N, T>& image) {
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    int new_rows = rows / 2;
    int new_cols = cols / 2;

    Image<N, T> result(new_rows, new_cols);
    for (int i = 0; i < new_rows; ++i) {
        for (int j = 0; j < new_cols; ++j) {
            result(i, j) = image.at(i * 2, j * 2);
        }
    }
    return result;
}

// ==================== 上采样 2×（复制像素填充） ====================

template<std::size_t N, typename T = uint8_t>
Image<N, T> upsample2(const Image<N, T>& image) {
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());

    Image<N, T> result(rows * 2, cols * 2);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            auto p = image.at(i, j);
            result(i * 2, j * 2) = p;
            result(i * 2, j * 2 + 1) = p;
            result(i * 2 + 1, j * 2) = p;
            result(i * 2 + 1, j * 2 + 1) = p;
        }
    }
    return result;
}

// ==================== 近似金字塔（高斯金字塔） ====================

template<std::size_t N, typename T = uint8_t>
std::vector<Image<N, T>> buildApproximationPyramid(const Image<N, T>& image, int levels) {
    std::vector<Image<N, T>> pyramid;
    pyramid.push_back(image); // 第J级 = 原图

    for (int l = 1; l <= levels; ++l) {
        auto blurred = gaussianBlur(pyramid[l - 1]);
        auto down = downsample2(blurred);
        pyramid.push_back(down);
    }

    return pyramid;
}

// ==================== 预测残差金字塔（拉普拉斯金字塔） ====================

template<std::size_t N, typename T = uint8_t>
std::vector<Image<N, T>> buildResidualPyramid(const Image<N, T>& image, int levels) {
    auto approx = buildApproximationPyramid(image, levels);

    std::vector<Image<N, T>> residual(levels + 1);
    // 最顶层残差 = 最顶层近似（最粗糙的近似作为残差保留）
    residual[levels] = approx[levels];

    for (int l = 0; l < levels; ++l) {
        // 将第 l 级近似上采样并平滑得到预测值
        auto upsampled = upsample2(approx[l + 1]);
        auto predicted = gaussianBlur(upsampled);

        int pred_rows = static_cast<int>(predicted.rows());
        int pred_cols = static_cast<int>(predicted.cols());

        // 确保预测图像尺寸与第 l 级近似相同
        int tgt_rows = static_cast<int>(approx[l].rows());
        int tgt_cols = static_cast<int>(approx[l].cols());

        Image<N, T> diff(tgt_rows, tgt_cols);
        for (int i = 0; i < tgt_rows; ++i) {
            for (int j = 0; j < tgt_cols; ++j) {
                Pixel<N, T> dp;
                for (std::size_t c = 0; c < N; ++c) {
                    float pred_val = (i < pred_rows && j < pred_cols)
                        ? static_cast<float>(predicted.at(i, j)[c])
                        : static_cast<float>(approx[l].at(i, j)[c]);
                    float orig_val = static_cast<float>(approx[l].at(i, j)[c]);
                    float diff_val = orig_val - pred_val + 128.0f; // 居中以便显示
                    if constexpr (std::is_same_v<T, uint8_t>)
                        dp[c] = static_cast<T>(std::clamp(std::round(diff_val), 0.0f, 255.0f));
                    else
                        dp[c] = static_cast<T>(diff_val);
                }
                diff(i, j) = dp;
            }
        }
        residual[l] = diff;
    }

    return residual;
}
