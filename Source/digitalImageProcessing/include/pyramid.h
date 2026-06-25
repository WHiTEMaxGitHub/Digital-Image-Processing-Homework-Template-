#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <vector>

inline std::vector<float> gaussianKernel1D() {
    return {1.0f / 16.0f, 4.0f / 16.0f, 6.0f / 16.0f, 4.0f / 16.0f, 1.0f / 16.0f};
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> gaussianBlur(const Image<N, T>& image) { return image; }

template<std::size_t N, typename T = uint8_t>
Image<N, T> downsample2(const Image<N, T>& image) {
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows / 2, cols / 2);
    for (int i = 0; i < rows / 2; ++i)
        for (int j = 0; j < cols / 2; ++j) result(i, j) = image.at(i * 2, j * 2);
    return result;
}

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

template<std::size_t N, typename T = uint8_t>
std::vector<Image<N, T>> buildApproximationPyramid(const Image<N, T>& image, int levels) {
    return {image};
}

template<std::size_t N, typename T = uint8_t>
std::vector<Image<N, T>> buildResidualPyramid(const Image<N, T>& image, int levels) {
    return {image};
}
