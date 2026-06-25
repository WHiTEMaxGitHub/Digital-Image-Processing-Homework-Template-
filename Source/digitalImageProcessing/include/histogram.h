#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

inline std::vector<float> computeCDF(const std::vector<size_t>& hist, size_t total_pixels) {
    std::vector<float> cdf(256, 0.0f);
    if (total_pixels == 0) return cdf;
    float sum = 0.0f;
    for (int i = 0; i < 256; ++i) { sum += hist[i]; cdf[i] = sum / total_pixels; }
    return cdf;
}

template<std::size_t N, typename T = uint8_t>
bool imageEqual(const Image<N, T>& a, const Image<N, T>& b) {
    if (a.rows() != b.rows() || a.cols() != b.cols()) return false;
    for (size_t i = 0; i < a.rows(); ++i)
        for (size_t j = 0; j < a.cols(); ++j)
            for (std::size_t c = 0; c < N; ++c)
                if (a.at(i, j)[c] != b.at(i, j)[c]) return false;
    return true;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> histogramEqualization(const Image<N, T>& image) {
    static_assert(std::is_same_v<T, uint8_t>);
    return Image<N, T>(0, 0);
}

template<std::size_t N, typename T = uint8_t>
std::pair<Image<N, T>, int> histogramEqualizationIterative(
    const Image<N, T>& image, int max_iterations = -1) {
    static_assert(std::is_same_v<T, uint8_t>);
    return {image, 0};
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> histogramEqualizationN(const Image<N, T>& image, int n) {
    static_assert(std::is_same_v<T, uint8_t>);
    return image;
}

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
