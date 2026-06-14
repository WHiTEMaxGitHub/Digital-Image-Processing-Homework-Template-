#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>

// 计算累积分布函数 (CDF)
inline std::vector<float> computeCDF(const std::vector<size_t>& hist, size_t total_pixels) {
    std::vector<float> cdf(256, 0.0f);
    if (total_pixels == 0) return cdf;
    
    float sum = 0.0f;
    for (int i = 0; i < 256; ++i) {
        sum += hist[i];
        cdf[i] = sum / total_pixels;
    }
    return cdf;
}

// 直方图均衡化
template<std::size_t N, typename T = uint8_t>
Image<N, T> histogramEqualization(const Image<N, T>& image) {
    static_assert(std::is_same_v<T, uint8_t>, "Only supports uint8_t type");
    
    size_t total_pixels = image.rows() * image.cols();
    
    // 计算所有通道的直方图
    std::vector<std::vector<size_t>> histograms(N, std::vector<size_t>(256, 0));
    
    for (size_t i = 0; i < image.rows(); ++i) {
        for (size_t j = 0; j < image.cols(); ++j) {
            auto pixel = image.at(i, j);
            for (std::size_t c = 0; c < N; ++c) {
                histograms[c][pixel[c]]++;
            }
        }
    }
    
    // 为每个通道计算映射表
    std::vector<std::vector<uint8_t>> mappings(N, std::vector<uint8_t>(256));
    for (std::size_t c = 0; c < N; ++c) {
        float cdf_sum = 0.0f;
        for (int i = 0; i < 256; ++i) {
            cdf_sum += static_cast<float>(histograms[c][i]) / total_pixels;
            mappings[c][i] = static_cast<uint8_t>(std::round(cdf_sum * 255.0f));
        }
    }
    
    // 应用映射
    Image<N, uint8_t> result(image.rows(), image.cols());
    for (size_t i = 0; i < image.rows(); ++i) {
        for (size_t j = 0; j < image.cols(); ++j) {
            auto pixel = image.at(i, j);
            for (std::size_t c = 0; c < N; ++c) {
                result(i, j)[c] = mappings[c][pixel[c]];
            }
        }
    }
    
    return result;
}

// 显示直方图（用于调试，返回文本表示）
template<typename T = uint8_t>
void printHistogram(const std::vector<size_t>& hist, int max_width = 80) {
    size_t max_count = *std::max_element(hist.begin(), hist.end());
    if (max_count == 0) return;
    
    for (int i = 0; i < 256; i += 16) { // 每16个灰度级显示一行
        std::cout << "Values " << i << "-" << std::min(i + 15, 255) << ": ";
        int bar_width = static_cast<int>(static_cast<float>(hist[i]) / max_count * max_width);
        std::cout << std::string(bar_width, '#') << " (" << hist[i] << ")" << std::endl;
    }
}