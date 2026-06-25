#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>

struct WaveletFilter {
    std::vector<float> lo_d, hi_d, lo_r, hi_r;
};

inline WaveletFilter haarFilter() {
    float s = 1.0f / std::sqrt(2.0f);
    return {{ s, s}, {-s, s}, { s, s}, { s, -s}};
}

inline WaveletFilter db4Filter() {
    float a = (1.0f + std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    float b = (3.0f + std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    float c = (3.0f - std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    float d = (1.0f - std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    return {{a, b, c, d}, {d, -c, b, -a}, {d, c, b, a}, {a, -b, c, -d}};
}

inline std::pair<std::vector<float>, std::vector<float>>
fwt1d(const std::vector<float>& signal,
      const std::vector<float>& lo_d,
      const std::vector<float>& hi_d) {
    int n = static_cast<int>(signal.size()), klen = static_cast<int>(lo_d.size());
    int out_len = (n + klen - 1) / 2;
    auto extended = signal;
    extended.insert(extended.end(), signal.begin(), signal.begin() + klen - 1);
    std::vector<float> approx(out_len), detail(out_len);
    for (int i = 0; i < out_len; ++i) {
        float sa = 0.0f, sd = 0.0f;
        for (int j = 0; j < klen; ++j) {
            int idx = 2 * i + j;
            sa += extended[idx] * lo_d[j]; sd += extended[idx] * hi_d[j];
        }
        approx[i] = sa; detail[i] = sd;
    }
    return {approx, detail};
}

template<std::size_t N>
struct WaveletDecomp {
    Image<1, float> LL[N], LH[N], HL[N], HH[N];
};

template<std::size_t N, typename T>
WaveletDecomp<N> fwt2d_single(const Image<N, T>& image, const WaveletFilter& wf) {
    static_assert(std::is_same_v<T, uint8_t>);
    return {};
}

inline Image<1, uint8_t> subbandToImage(const Image<1, float>& band) {
    int rows = static_cast<int>(band.rows()), cols = static_cast<int>(band.cols());
    float max_val = 0.0f;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            max_val = std::max(max_val, std::abs(band.at(i, j)[0]));
    Image<1, uint8_t> result(rows, cols);
    float scale = (max_val > 0.0f) ? 255.0f / max_val : 0.0f;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result(i, j)[0] = static_cast<uint8_t>(
                std::clamp(std::round(std::abs(band.at(i, j)[0]) * scale), 0.0f, 255.0f));
    return result;
}

template<std::size_t N>
Image<1, uint8_t> assembleWaveletDisplay(const WaveletDecomp<N>& dec, std::size_t ch = 0) {
    auto im_LL = subbandToImage(dec.LL[ch]), im_LH = subbandToImage(dec.LH[ch]);
    auto im_HL = subbandToImage(dec.HL[ch]), im_HH = subbandToImage(dec.HH[ch]);
    int r = static_cast<int>(im_LL.rows()), c = static_cast<int>(im_LL.cols());
    Image<1, uint8_t> result(r * 2, c * 2);
    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j) {
            result(i, j)[0] = im_LL.at(i, j)[0]; result(i, j + c)[0] = im_LH.at(i, j)[0];
            result(i + r, j)[0] = im_HL.at(i, j)[0]; result(i + r, j + c)[0] = im_HH.at(i, j)[0];
        }
    return result;
}

template<std::size_t N, typename T>
Image<1, uint8_t> fwt2d_multilevel(const Image<N, T>& image,
                                    const WaveletFilter& wf, int levels,
                                    std::size_t channel = 0) {
    static_assert(std::is_same_v<T, uint8_t>);
    return Image<1, uint8_t>(0, 0);
}

template<std::size_t N, typename T>
Image<N, uint8_t> waveletEdgeDetection(const Image<N, T>& image,
                                         const WaveletFilter& wf, int levels,
                                         float threshold_factor = 1.5f) {
    static_assert(std::is_same_v<T, uint8_t>);
    return Image<N, uint8_t>(0, 0);
}
