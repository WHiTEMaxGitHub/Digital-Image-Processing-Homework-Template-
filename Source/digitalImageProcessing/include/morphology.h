#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>

// ==================== 结构元素 ====================

struct StructuringElement {
    int rows, cols;
    int origin_r, origin_c;
    std::vector<std::vector<bool>> data;
};

inline StructuringElement rectSE(int rows, int cols) {
    StructuringElement se{rows, cols, rows / 2, cols / 2, {}};
    se.data.assign(rows, std::vector<bool>(cols, true));
    return se;
}
inline StructuringElement hlineSE(int length) {
    StructuringElement se{1, length, 0, length / 2, {}};
    se.data.assign(1, std::vector<bool>(length, true));
    return se;
}
inline StructuringElement vlineSE(int length) {
    StructuringElement se{length, 1, length / 2, 0, {}};
    se.data.assign(length, std::vector<bool>(1, true));
    return se;
}
inline StructuringElement crossSE(int size) {
    StructuringElement se{size, size, size / 2, size / 2, {}};
    se.data.assign(size, std::vector<bool>(size, false));
    int c = size / 2;
    for (int i = 0; i < size; ++i) { se.data[i][c] = true; se.data[c][i] = true; }
    return se;
}
inline StructuringElement diskSE(int radius) {
    int size = 2 * radius + 1;
    StructuringElement se{size, size, radius, radius, {}};
    se.data.assign(size, std::vector<bool>(size, false));
    int r2 = radius * radius;
    for (int i = -radius; i <= radius; ++i)
        for (int j = -radius; j <= radius; ++j)
            if (i * i + j * j <= r2) se.data[i + radius][j + radius] = true;
    return se;
}

// ==================== 二值形态学 ====================

template<std::size_t N, typename T = uint8_t>
Image<N, T> threshold(const Image<N, T>& image, uint8_t th = 128) {
    static_assert(std::is_same_v<T, uint8_t>);
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c)
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                result(i, j)[c] = (image.at(i, j)[c] >= th) ? 255 : 0;
    return result;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryDilate(const Image<N, T>& image, const StructuringElement& se) {
    static_assert(std::is_same_v<T, uint8_t>);
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c)
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) {
                bool hit = false;
                for (int si = 0; si < se.rows && !hit; ++si)
                    for (int sj = 0; sj < se.cols && !hit; ++sj) {
                        if (!se.data[si][sj]) continue;
                        int ni = i + si - se.origin_r, nj = j + sj - se.origin_c;
                        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols
                            && image.at(ni, nj)[c] == 255) hit = true;
                    }
                result(i, j)[c] = hit ? 255 : 0;
            }
    return result;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryErode(const Image<N, T>& image, const StructuringElement& se) {
    static_assert(std::is_same_v<T, uint8_t>);
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c)
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) {
                bool all_fit = true;
                for (int si = 0; si < se.rows && all_fit; ++si)
                    for (int sj = 0; sj < se.cols && all_fit; ++sj) {
                        if (!se.data[si][sj]) continue;
                        int ni = i + si - se.origin_r, nj = j + sj - se.origin_c;
                        if (ni < 0 || ni >= rows || nj < 0 || nj >= cols
                            || image.at(ni, nj)[c] != 255) all_fit = false;
                    }
                result(i, j)[c] = all_fit ? 255 : 0;
            }
    return result;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryOpen(const Image<N, T>& image, const StructuringElement& se) {
    return binaryDilate(binaryErode(image, se), se);
}
template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryClose(const Image<N, T>& image, const StructuringElement& se) {
    return binaryErode(binaryDilate(image, se), se);
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryNot(const Image<N, T>& image) {
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c)
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                result(i, j)[c] = (image.at(i, j)[c] == 255) ? 0 : 255;
    return result;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> binaryAnd(const Image<N, T>& a, const Image<N, T>& b) {
    int rows = static_cast<int>(a.rows()), cols = static_cast<int>(a.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c)
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                result(i, j)[c] = (a.at(i, j)[c] == 255 && b.at(i, j)[c] == 255) ? 255 : 0;
    return result;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> geodesicDilate(const Image<N, T>& marker, const Image<N, T>& mask,
                            const StructuringElement& se) {
    return binaryAnd(binaryDilate(marker, se), mask);
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> morphReconstruct(const Image<N, T>& marker, const Image<N, T>& mask,
                              const StructuringElement& se, int max_iter = -1) {
    auto current = marker;
    int iter = 0;
    while (true) {
        auto next = geodesicDilate(current, mask, se);
        iter++;
        bool changed = false;
        for (int i = 0; i < static_cast<int>(current.rows()) && !changed; ++i)
            for (int j = 0; j < static_cast<int>(current.cols()) && !changed; ++j)
                if (current.at(i, j) != next.at(i, j)) changed = true;
        if (!changed) return next;
        current = next;
        if (max_iter > 0 && iter >= max_iter) return current;
    }
}

// ==================== 图9.29：长字符提取 ====================

template<std::size_t N, typename T = uint8_t>
Image<N, T> longCharacterExtraction(const Image<N, T>& binary, int length = 51) {
    return binaryOpen(binary, hlineSE(length));
}

// ==================== 图9.31：空洞填充 ====================

template<std::size_t N, typename T = uint8_t>
Image<N, T> holeFilling(const Image<N, T>& binary) {
    int rows = static_cast<int>(binary.rows()), cols = static_cast<int>(binary.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c) {
        // 提取单通道
        Image<1, T> ch(rows, cols);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) ch(i, j)[0] = binary.at(i, j)[c];
        auto comp = binaryNot(ch);
        Image<1, T> marker(rows, cols);
        for (int i = 0; i < rows; ++i) {
            marker(i, 0)[0] = comp.at(i, 0)[0];
            marker(i, cols-1)[0] = comp.at(i, cols-1)[0];
        }
        for (int j = 0; j < cols; ++j) {
            marker(0, j)[0] = comp.at(0, j)[0];
            marker(rows-1, j)[0] = comp.at(rows-1, j)[0];
        }
        auto filled = binaryNot(morphReconstruct(marker, comp, crossSE(3)));
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) result(i, j)[c] = filled.at(i, j)[0];
    }
    return result;
}

// ==================== 图9.32：边界清除 ====================

template<std::size_t N, typename T = uint8_t>
Image<N, T> borderClearing(const Image<N, T>& binary) {
    int rows = static_cast<int>(binary.rows()), cols = static_cast<int>(binary.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c) {
        Image<1, T> ch(rows, cols);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) ch(i, j)[0] = binary.at(i, j)[c];
        Image<1, T> marker(rows, cols);
        for (int i = 0; i < rows; ++i) {
            marker(i, 0)[0] = ch.at(i, 0)[0];
            marker(i, cols-1)[0] = ch.at(i, cols-1)[0];
        }
        for (int j = 0; j < cols; ++j) {
            marker(0, j)[0] = ch.at(0, j)[0];
            marker(rows-1, j)[0] = ch.at(rows-1, j)[0];
        }
        auto border_objs = morphReconstruct(marker, ch, crossSE(3));
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                result(i, j)[c] = (ch.at(i, j)[0] == 255 && border_objs.at(i, j)[0] == 0) ? 255 : 0;
    }
    return result;
}

// ==================== 灰度形态学 ====================

template<std::size_t N, typename T = uint8_t>
Image<N, T> grayDilate(const Image<N, T>& image, const StructuringElement& se) {
    static_assert(std::is_same_v<T, uint8_t>);
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c)
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) {
                uint8_t max_val = 0; bool first = true;
                for (int si = 0; si < se.rows; ++si)
                    for (int sj = 0; sj < se.cols; ++sj) {
                        if (!se.data[si][sj]) continue;
                        int ni = i + si - se.origin_r, nj = j + sj - se.origin_c;
                        if (ni < 0 || ni >= rows || nj < 0 || nj >= cols) continue;
                        uint8_t v = image.at(ni, nj)[c];
                        if (first || v > max_val) { max_val = v; first = false; }
                    }
                result(i, j)[c] = first ? image.at(i, j)[c] : max_val;
            }
    return result;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> grayErode(const Image<N, T>& image, const StructuringElement& se) {
    static_assert(std::is_same_v<T, uint8_t>);
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c)
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) {
                uint8_t min_val = 255; bool first = true;
                for (int si = 0; si < se.rows; ++si)
                    for (int sj = 0; sj < se.cols; ++sj) {
                        if (!se.data[si][sj]) continue;
                        int ni = i + si - se.origin_r, nj = j + sj - se.origin_c;
                        if (ni < 0 || ni >= rows || nj < 0 || nj >= cols) continue;
                        uint8_t v = image.at(ni, nj)[c];
                        if (first || v < min_val) { min_val = v; first = false; }
                    }
                result(i, j)[c] = first ? image.at(i, j)[c] : min_val;
            }
    return result;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> grayOpen(const Image<N, T>& image, const StructuringElement& se) {
    return grayDilate(grayErode(image, se), se);
}
template<std::size_t N, typename T = uint8_t>
Image<N, T> grayClose(const Image<N, T>& image, const StructuringElement& se) {
    return grayErode(grayDilate(image, se), se);
}

// ==================== 图9.40：顶帽变换 ====================

template<std::size_t N, typename T = uint8_t>
Image<N, T> topHatTransform(const Image<N, T>& image, const StructuringElement& se) {
    auto opened = grayOpen(image, se);
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c)
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                result(i, j)[c] = static_cast<T>(std::clamp(
                    static_cast<int>(image.at(i, j)[c]) - opened.at(i, j)[c], 0, 255));
    return result;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> topHatCorrect(const Image<N, T>& image, int se_size = 40) {
    return topHatTransform(image, diskSE(se_size));
}

// ==================== 图9.41：粒度测定 ====================

template<std::size_t N, typename T = uint8_t>
std::vector<int> granulometry(const Image<N, T>& binary, int max_radius = 30) {
    std::vector<int> surface_areas; int initial = 0;
    for (int i = 0; i < static_cast<int>(binary.rows()); ++i)
        for (int j = 0; j < static_cast<int>(binary.cols()); ++j)
            for (std::size_t c = 0; c < N; ++c)
                if (binary.at(i, j)[c] == 255) initial++;
    surface_areas.push_back(initial);
    auto current = binary;
    for (int r = 1; r <= max_radius; ++r) {
        auto opened = binaryOpen(current, diskSE(r));
        int area = 0;
        for (int i = 0; i < static_cast<int>(opened.rows()); ++i)
            for (int j = 0; j < static_cast<int>(opened.cols()); ++j)
                for (std::size_t c = 0; c < N; ++c)
                    if (opened.at(i, j)[c] == 255) area++;
        surface_areas.push_back(area);
        current = opened;
    }
    return surface_areas;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> granulometryVisualize(const Image<N, T>& binary, int max_radius = 30) {
    int rows = static_cast<int>(binary.rows()), cols = static_cast<int>(binary.cols());
    Image<N, T> result(rows, cols);
    auto current = binary;
    for (std::size_t c = 0; c < N; ++c) {
        auto ch = current; // 每个通道独立处理
        for (int r = 1; r <= max_radius; ++r) {
            auto opened = binaryOpen(ch, diskSE(r));
            uint8_t shade = static_cast<uint8_t>(std::min(255, r * 10));
            for (int i = 0; i < rows; ++i)
                for (int j = 0; j < cols; ++j)
                    if (ch.at(i, j)[c] == 255 && opened.at(i, j)[c] == 0)
                        result(i, j)[c] = shade;
            ch = opened;
        }
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                if (ch.at(i, j)[c] == 255) result(i, j)[c] = 255;
    }
    return result;
}

// ==================== 图9.43：分水岭预处理 ====================

template<std::size_t N, typename T = uint8_t>
Image<N, T> morphGradient(const Image<N, T>& image, const StructuringElement& se) {
    auto dilated = grayDilate(image, se), eroded = grayErode(image, se);
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c)
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                result(i, j)[c] = static_cast<T>(std::clamp(
                    static_cast<int>(dilated.at(i, j)[c]) - eroded.at(i, j)[c], 0, 255));
    return result;
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> watershedPreprocess(const Image<N, T>& image,
                                  int smooth_radius = 3, int grad_radius = 3) {
    auto se_smooth = diskSE(smooth_radius);
    auto smoothed = grayClose(grayOpen(image, se_smooth), se_smooth);
    return morphGradient(smoothed, diskSE(grad_radius));
}

template<std::size_t N, typename T = uint8_t>
Image<N, T> watershedMarkers(const Image<N, T>& image, int min_distance = 10) {
    int rows = static_cast<int>(image.rows()), cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    for (std::size_t c = 0; c < N; ++c) {
        // 提取单通道，逐通道 Otsu
        Image<1, T> ch(rows, cols);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) ch(i, j)[0] = image.at(i, j)[c];
        auto opened = grayOpen(ch, diskSE(3));
        auto closed = grayClose(opened, diskSE(min_distance));

        int hist[256] = {};
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) hist[closed.at(i, j)[0]]++;
        int total = rows * cols;
        float sum = 0; for (int t = 0; t < 256; ++t) sum += t * hist[t];
        float sumB = 0; int wB = 0; float max_var = 0; int thresh = 128;
        for (int t = 0; t < 256; ++t) {
            wB += hist[t]; if (wB == 0) continue;
            int wF = total - wB; if (wF == 0) break;
            sumB += t * hist[t];
            float mB = sumB / wB, mF = (sum - sumB) / wF;
            float var_b = static_cast<float>(wB) * wF * (mB - mF) * (mB - mF);
            if (var_b > max_var) { max_var = var_b; thresh = t; }
        }
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                result(i, j)[c] = (closed.at(i, j)[0] >= thresh) ? 255 : 0;
    }
    return result;
}
