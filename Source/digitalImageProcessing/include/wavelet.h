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
    std::vector<float> lo_d;  // 分解低通
    std::vector<float> hi_d;  // 分解高通
    std::vector<float> lo_r;  // 重构低通
    std::vector<float> hi_r;  // 重构高通
};

// Haar 小波
inline WaveletFilter haarFilter() {
    float s = 1.0f / std::sqrt(2.0f);
    return {
        { s,  s},
        {-s,  s},
        { s,  s},
        { s, -s}
    };
}

// Daubechies 4 (db4)
inline WaveletFilter db4Filter() {
    float a = (1.0f + std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    float b = (3.0f + std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    float c = (3.0f - std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    float d = (1.0f - std::sqrt(3.0f)) / (4.0f * std::sqrt(2.0f));
    return {
        {a, b, c, d},
        {d, -c, b, -a},
        {d, c, b, a},
        {a, -b, c, -d}
    };
}

// ==================== 一维卷积（周期边界） ====================

inline std::vector<float> conv1d(const std::vector<float>& signal,
                                  const std::vector<float>& kernel) {
    int n = static_cast<int>(signal.size());
    int klen = static_cast<int>(kernel.size());
    std::vector<float> result(n + klen - 1, 0.0f);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < klen; ++j) {
            result[i + j] += signal[i] * kernel[j];
        }
    }
    return result;
}

// ==================== 一维 FWT（分解，周期扩展） ====================

inline std::pair<std::vector<float>, std::vector<float>>
fwt1d(const std::vector<float>& signal,
      const std::vector<float>& lo_d,
      const std::vector<float>& hi_d) {
    int n = static_cast<int>(signal.size());
    int klen = static_cast<int>(lo_d.size());
    int out_len = (n + klen - 1) / 2; // 下采样后长度

    // 周期扩展
    auto extended = signal;
    extended.insert(extended.end(), signal.begin(), signal.begin() + klen - 1);

    std::vector<float> approx(out_len);
    std::vector<float> detail(out_len);

    for (int i = 0; i < out_len; ++i) {
        float sa = 0.0f, sd = 0.0f;
        for (int j = 0; j < klen; ++j) {
            int idx = 2 * i + j;
            sa += extended[idx] * lo_d[j];
            sd += extended[idx] * hi_d[j];
        }
        approx[i] = sa;
        detail[i] = sd;
    }

    return {approx, detail};
}

// ==================== 二维 FWT ====================

struct WaveletDecomp {
    Image<1, float> LL, LH, HL, HH;
};

// 单层二维快速小波变换
inline WaveletDecomp fwt2d_single(const Image<1, uint8_t>& image,
                                   const WaveletFilter& wf) {
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    int hrows = (rows + static_cast<int>(wf.lo_d.size()) - 1) / 2;
    int hcols = (cols + static_cast<int>(wf.lo_d.size()) - 1) / 2;

    // 行变换：中间结果 L, H 各 (hrows, cols)
    std::vector<std::vector<float>> rowL(rows, std::vector<float>(hcols));
    std::vector<std::vector<float>> rowH(rows, std::vector<float>(hcols));

    for (int i = 0; i < rows; ++i) {
        std::vector<float> row(cols);
        for (int j = 0; j < cols; ++j) row[j] = static_cast<float>(image.at(i, j)[0]);
        auto [a, d] = fwt1d(row, wf.lo_d, wf.hi_d);
        rowL[i] = a;
        rowH[i] = d;
    }

    // 列变换
    WaveletDecomp result;
    result.LL = Image<1, float>(hrows, hcols);
    result.LH = Image<1, float>(hrows, hcols);
    result.HL = Image<1, float>(hrows, hcols);
    result.HH = Image<1, float>(hrows, hcols);

    for (int j = 0; j < hcols; ++j) {
        std::vector<float> colL(rows), colH(rows);
        for (int i = 0; i < rows; ++i) { colL[i] = rowL[i][j]; colH[i] = rowH[i][j]; }
        auto [a_L, d_L] = fwt1d(colL, wf.lo_d, wf.hi_d);
        auto [a_H, d_H] = fwt1d(colH, wf.lo_d, wf.hi_d);
        for (int i = 0; i < hrows; ++i) {
            result.LL(i, j)[0] = a_L[i];
            result.LH(i, j)[0] = d_L[i];
            result.HL(i, j)[0] = a_H[i];
            result.HH(i, j)[0] = d_H[i];
        }
    }

    return result;
}

// 子带归一化到 uint8_t 以便显示
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

// 将四个子带拼接成 2×2 显示图
inline Image<1, uint8_t> assembleWaveletDisplay(const WaveletDecomp& dec) {
    auto im_LL = subbandToImage(dec.LL);
    auto im_LH = subbandToImage(dec.LH);
    auto im_HL = subbandToImage(dec.HL);
    auto im_HH = subbandToImage(dec.HH);

    int r = static_cast<int>(im_LL.rows());
    int c = static_cast<int>(im_LL.cols());
    Image<1, uint8_t> result(r * 2, c * 2);

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            result(i, j)[0] = im_LL.at(i, j)[0];
            result(i, j + c)[0] = im_LH.at(i, j)[0];
            result(i + r, j)[0] = im_HL.at(i, j)[0];
            result(i + r, j + c)[0] = im_HH.at(i, j)[0];
        }
    }
    return result;
}

// 多级二维快速小波变换，返回拼接显示图
inline Image<1, uint8_t> fwt2d_multilevel(const Image<1, uint8_t>& image,
                                            const WaveletFilter& wf,
                                            int levels) {
    if (levels <= 0) return image;

    // 递归分解 LL 子带
    std::vector<WaveletDecomp> decomps;
    Image<1, uint8_t> current = image;

    for (int l = 0; l < levels; ++l) {
        auto dec = fwt2d_single(current, wf);
        decomps.push_back(dec);
        // 将 LL 转为 uint8_t 作为下一级输入
        current = subbandToImage(dec.LL);
    }

    // 从最深层开始逐层组装：最后一级作为左上角，逐级扩展
    Image<1, uint8_t> result = assembleWaveletDisplay(decomps.back());

    for (int l = static_cast<int>(decomps.size()) - 2; l >= 0; --l) {
        auto im_LH = subbandToImage(decomps[l].LH);
        auto im_HL = subbandToImage(decomps[l].HL);
        auto im_HH = subbandToImage(decomps[l].HH);

        int r = static_cast<int>(result.rows());
        int c = static_cast<int>(result.cols());
        int hr = static_cast<int>(im_LH.rows());
        int hc = static_cast<int>(im_LH.cols());

        // 左上保留 result，其余三个角填充
        Image<1, uint8_t> composite(r + hr, c + hc);

        // 复制 result 到左上
        for (int i = 0; i < r; ++i)
            for (int j = 0; j < c; ++j)
                composite(i, j)[0] = result.at(i, j)[0];

        // 右上 = LH
        for (int i = 0; i < hr; ++i)
            for (int j = 0; j < hc; ++j)
                composite(i, j + c)[0] = im_LH.at(i, j)[0];

        // 左下 = HL
        for (int i = 0; i < hr; ++i)
            for (int j = 0; j < hc; ++j)
                composite(i + r, j)[0] = im_HL.at(i, j)[0];

        // 右下 = HH
        for (int i = 0; i < hr; ++i)
            for (int j = 0; j < hc; ++j)
                composite(i + r, j + c)[0] = im_HH.at(i, j)[0];

        result = composite;
    }

    return result;
}

// ==================== 小波边缘检测（例7.13） ====================

inline Image<1, uint8_t> waveletEdgeDetection(const Image<1, uint8_t>& image,
                                                const WaveletFilter& wf,
                                                int levels,
                                                float threshold_factor = 1.5f) {
    // 执行一级小波分解
    auto dec = fwt2d_single(image, wf);

    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<1, float> edge_map(rows, cols);

    // 上采样高频子带回原尺寸（简单复制）
    int hr = static_cast<int>(dec.LH.rows());
    int hc = static_cast<int>(dec.LH.cols());

    // 计算边缘强度：综合 LH、HL、HH
    float max_edge = 0.0f;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int si = std::min(i / 2, hr - 1);
            int sj = std::min(j / 2, hc - 1);
            float lh = std::abs(dec.LH.at(si, sj)[0]);
            float hl = std::abs(dec.HL.at(si, sj)[0]);
            float hh = std::abs(dec.HH.at(si, sj)[0]);
            float edge_val = std::sqrt(lh * lh + hl * hl + hh * hh);
            edge_map(i, j)[0] = edge_val;
            max_edge = std::max(max_edge, edge_val);
        }
    }

    // 阈值处理
    float threshold = (max_edge > 0.0f) ? threshold_factor * max_edge / 10.0f : 0.0f;

    Image<1, uint8_t> result(rows, cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float v = edge_map.at(i, j)[0];
            if (v >= threshold)
                result(i, j)[0] = 255;
            else
                result(i, j)[0] = static_cast<uint8_t>(
                    std::clamp(std::round(v / threshold * 128.0f), 0.0f, 255.0f));
        }
    }

    return result;
}
