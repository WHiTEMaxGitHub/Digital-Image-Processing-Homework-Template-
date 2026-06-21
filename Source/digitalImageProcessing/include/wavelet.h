#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>

// ==================== 小波滤波器定义 ====================

struct WaveletFilter {
    std::vector<float> lo_d;
    std::vector<float> hi_d;
    std::vector<float> lo_r;
    std::vector<float> hi_r;
};

inline WaveletFilter haarFilter() {
    float s = 1.0f / std::sqrt(2.0f);
    return {{ s,  s}, {-s,  s}, { s,  s}, { s, -s}};
}

inline WaveletFilter db4Filter() {
    float a = (1.0f + std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    float b = (3.0f + std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    float c = (3.0f - std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    float d = (1.0f - std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    return {{a, b, c, d}, {d, -c, b, -a}, {d, c, b, a}, {a, -b, c, -d}};
}

// ==================== 一维 FWT ====================

inline std::pair<std::vector<float>, std::vector<float>>
fwt1d(const std::vector<float>& signal,
      const std::vector<float>& lo_d,
      const std::vector<float>& hi_d) {
    int n = static_cast<int>(signal.size());
    int klen = static_cast<int>(lo_d.size());
    int out_len = (n + klen - 1) / 2;

    auto extended = signal;
    extended.insert(extended.end(), signal.begin(), signal.begin() + klen - 1);

    std::vector<float> approx(out_len), detail(out_len);
    for (int i = 0; i < out_len; ++i) {
        float sa = 0.0f, sd = 0.0f;
        for (int j = 0; j < klen; ++j) {
            int idx = 2 * i + j;
            sa += extended[idx] * lo_d[j];
            sd += extended[idx] * hi_d[j];
        }
        approx[i] = sa; detail[i] = sd;
    }
    return {approx, detail};
}

// ==================== 二维 FWT（模板化 N 通道） ====================

template<std::size_t N>
struct WaveletDecomp {
    Image<1, float> LL[N]; // 每个通道的分解结果
    Image<1, float> LH[N];
    Image<1, float> HL[N];
    Image<1, float> HH[N];
};

template<std::size_t N, typename T>
WaveletDecomp<N> fwt2d_single(const Image<N, T>& image,
                               const WaveletFilter& wf) {
    static_assert(std::is_same_v<T, uint8_t>, "Only supports uint8_t type");

    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    int hrows = (rows + static_cast<int>(wf.lo_d.size()) - 1) / 2;
    int hcols = (cols + static_cast<int>(wf.lo_d.size()) - 1) / 2;

    WaveletDecomp<N> result;

    for (std::size_t c = 0; c < N; ++c) {
        // 行变换
        std::vector<std::vector<float>> rowL(rows, std::vector<float>(hcols));
        std::vector<std::vector<float>> rowH(rows, std::vector<float>(hcols));

        for (int i = 0; i < rows; ++i) {
            std::vector<float> row(cols);
            for (int j = 0; j < cols; ++j)
                row[j] = static_cast<float>(image.at(i, j)[c]);
            auto [a, d] = fwt1d(row, wf.lo_d, wf.hi_d);
            rowL[i] = a; rowH[i] = d;
        }

        // 列变换
        result.LL[c] = Image<1, float>(hrows, hcols);
        result.LH[c] = Image<1, float>(hrows, hcols);
        result.HL[c] = Image<1, float>(hrows, hcols);
        result.HH[c] = Image<1, float>(hrows, hcols);

        for (int j = 0; j < hcols; ++j) {
            std::vector<float> colL(rows), colH(rows);
            for (int i = 0; i < rows; ++i) {
                colL[i] = rowL[i][j]; colH[i] = rowH[i][j];
            }
            auto [a_L, d_L] = fwt1d(colL, wf.lo_d, wf.hi_d);
            auto [a_H, d_H] = fwt1d(colH, wf.lo_d, wf.hi_d);
            for (int i = 0; i < hrows; ++i) {
                result.LL[c](i, j)[0] = a_L[i];
                result.LH[c](i, j)[0] = d_L[i];
                result.HL[c](i, j)[0] = a_H[i];
                result.HH[c](i, j)[0] = d_H[i];
            }
        }
    }
    return result;
}

// ==================== 子带转图像 ====================

inline Image<1, uint8_t> subbandToImage(const Image<1, float>& band) {
    int rows = static_cast<int>(band.rows());
    int cols = static_cast<int>(band.cols());
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

// ==================== 拼接显示图 ====================

template<std::size_t N>
Image<1, uint8_t> assembleWaveletDisplay(const WaveletDecomp<N>& dec, std::size_t ch = 0) {
    auto im_LL = subbandToImage(dec.LL[ch]);
    auto im_LH = subbandToImage(dec.LH[ch]);
    auto im_HL = subbandToImage(dec.HL[ch]);
    auto im_HH = subbandToImage(dec.HH[ch]);

    int r = static_cast<int>(im_LL.rows());
    int c = static_cast<int>(im_LL.cols());
    Image<1, uint8_t> result(r * 2, c * 2);

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            result(i, j)[0]         = im_LL.at(i, j)[0];
            result(i, j + c)[0]     = im_LH.at(i, j)[0];
            result(i + r, j)[0]     = im_HL.at(i, j)[0];
            result(i + r, j + c)[0] = im_HH.at(i, j)[0];
        }
    }
    return result;
}

// ==================== 多级 FWT ====================

template<std::size_t N, typename T>
Image<1, uint8_t> fwt2d_multilevel(const Image<N, T>& image,
                                    const WaveletFilter& wf,
                                    int levels,
                                    std::size_t channel = 0) {
    static_assert(std::is_same_v<T, uint8_t>, "Only supports uint8_t type");
    if (levels <= 0) {
        // 单通道灰度
        Image<1, uint8_t> out(image.rows(), image.cols());
        for (int i = 0; i < static_cast<int>(image.rows()); ++i)
            for (int j = 0; j < static_cast<int>(image.cols()); ++j)
                out(i, j)[0] = image.at(i, j)[channel];
        return out;
    }

    // 逐通道、逐级分解
    // 为简化，对指定通道递归分解
    // 构建单通道中间图像序列
    std::vector<WaveletDecomp<1>> decomps;
    Image<1, uint8_t> current(image.rows(), image.cols());
    for (int i = 0; i < static_cast<int>(image.rows()); ++i)
        for (int j = 0; j < static_cast<int>(image.cols()); ++j)
            current(i, j)[0] = image.at(i, j)[channel];

    for (int l = 0; l < levels; ++l) {
        auto dec = fwt2d_single<1, uint8_t>(current, wf);
        decomps.push_back(dec);
        current = subbandToImage(dec.LL[0]);
    }

    Image<1, uint8_t> result = assembleWaveletDisplay<1>(decomps.back());

    for (int l = static_cast<int>(decomps.size()) - 2; l >= 0; --l) {
        auto im_LH = subbandToImage(decomps[l].LH[0]);
        auto im_HL = subbandToImage(decomps[l].HL[0]);
        auto im_HH = subbandToImage(decomps[l].HH[0]);

        int r = static_cast<int>(result.rows());
        int c = static_cast<int>(result.cols());
        int hr = static_cast<int>(im_LH.rows());
        int hc = static_cast<int>(im_LH.cols());

        Image<1, uint8_t> composite(r + hr, c + hc);
        for (int i = 0; i < r; ++i)
            for (int j = 0; j < c; ++j)
                composite(i, j)[0] = result.at(i, j)[0];
        for (int i = 0; i < hr; ++i)
            for (int j = 0; j < hc; ++j) {
                composite(i, j + c)[0]       = im_LH.at(i, j)[0];
                composite(i + r, j)[0]       = im_HL.at(i, j)[0];
                composite(i + r, j + c)[0]   = im_HH.at(i, j)[0];
            }
        result = composite;
    }
    return result;
}

// ==================== 小波边缘检测 ====================

template<std::size_t N, typename T>
Image<N, uint8_t> waveletEdgeDetection(const Image<N, T>& image,
                                         const WaveletFilter& wf,
                                         int levels,
                                         float threshold_factor = 1.5f) {
    static_assert(std::is_same_v<T, uint8_t>, "Only supports uint8_t type");

    auto dec = fwt2d_single(image, wf);
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<N, uint8_t> result(rows, cols);

    for (std::size_t c = 0; c < N; ++c) {
        int hr = static_cast<int>(dec.LH[c].rows());
        int hc = static_cast<int>(dec.LH[c].cols());

        float max_edge = 0.0f;
        Image<1, float> edge_map(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                int si = std::min(i / 2, hr - 1);
                int sj = std::min(j / 2, hc - 1);
                float lh = std::abs(dec.LH[c].at(si, sj)[0]);
                float hl = std::abs(dec.HL[c].at(si, sj)[0]);
                float hh = std::abs(dec.HH[c].at(si, sj)[0]);
                float ev = std::sqrt(lh * lh + hl * hl + hh * hh);
                edge_map(i, j)[0] = ev;
                max_edge = std::max(max_edge, ev);
            }
        }

        float thresh = (max_edge > 0.0f) ? threshold_factor * max_edge / 10.0f : 0.0f;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                float v = edge_map.at(i, j)[0];
                if (v >= thresh)
                    result(i, j)[c] = 255;
                else
                    result(i, j)[c] = static_cast<uint8_t>(
                        std::clamp(std::round(v / thresh * 128.0f), 0.0f, 255.0f));
            }
        }
    }
    return result;
}
