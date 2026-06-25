#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <algorithm>

inline float cubic_weight(float t) {
    t = std::abs(t);
    float a = -0.5f;
    if (t < 1.0f) return ((a + 2) * t - (a + 3)) * t * t + 1;
    if (t < 2.0f) return ((a * t - 5 * a) * t + 8 * a) * t - 4 * a;
    return 0.0f;
}

template <std::size_t N, typename T = uint8_t>
Image<N, T> nearestNeighborInterpolation(Image<N, T>* origin, float row_zoom, float col_zoom) {
    return Image<N, T>(0, 0);
}

template <std::size_t N, typename T = uint8_t>
Image<N, T> bi_LinearInterpolation(Image<N, T>* origin, float row_zoom, float col_zoom) {
    return Image<N, T>(0, 0);
}

template <std::size_t N, typename T>
Image<N, T> bi_CubicInterpolation(Image<N, T>* origin, float row_zoom, float col_zoom) {
    return Image<N, T>(0, 0);
}
