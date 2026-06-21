#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>

// ==================== 结构元素 ====================

struct StructuringElement {
    int rows, cols;
    int origin_r, origin_c;  // 原点（在 SE 坐标系中）
    std::vector<std::vector<bool>> data;
};

// 创建矩形结构元素（全 1）
inline StructuringElement rectSE(int rows, int cols) {
    StructuringElement se;
    se.rows = rows;
    se.cols = cols;
    se.origin_r = rows / 2;
    se.origin_c = cols / 2;
    se.data.assign(rows, std::vector<bool>(cols, true));
    return se;
}

// 创建水平线结构元素
inline StructuringElement hlineSE(int length) {
    StructuringElement se;
    se.rows = 1;
    se.cols = length;
    se.origin_r = 0;
    se.origin_c = length / 2;
    se.data.assign(1, std::vector<bool>(length, true));
    return se;
}

// 创建垂直线结构元素
inline StructuringElement vlineSE(int length) {
    StructuringElement se;
    se.rows = length;
    se.cols = 1;
    se.origin_r = length / 2;
    se.origin_c = 0;
    se.data.assign(length, std::vector<bool>(1, true));
    return se;
}

// 创建十字形结构元素
inline StructuringElement crossSE(int size) {
    StructuringElement se;
    se.rows = size;
    se.cols = size;
    se.origin_r = size / 2;
    se.origin_c = size / 2;
    se.data.assign(size, std::vector<bool>(size, false));
    int c = size / 2;
    for (int i = 0; i < size; ++i) {
        se.data[i][c] = true;
        se.data[c][i] = true;
    }
    return se;
}

// 创建圆盘形结构元素
inline StructuringElement diskSE(int radius) {
    int size = 2 * radius + 1;
    StructuringElement se;
    se.rows = size;
    se.cols = size;
    se.origin_r = radius;
    se.origin_c = radius;
    se.data.assign(size, std::vector<bool>(size, false));
    int r2 = radius * radius;
    for (int i = -radius; i <= radius; ++i)
        for (int j = -radius; j <= radius; ++j)
            if (i * i + j * j <= r2)
                se.data[i + radius][j + radius] = true;
    return se;
}

// ==================== 二值形态学 ====================

// 二值化（阈值处理，灰度 → 0/255）
inline Image<1, uint8_t> threshold(const Image<1, uint8_t>& image, uint8_t th = 128) {
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<1, uint8_t> result(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result(i, j)[0] = (image.at(i, j)[0] >= th) ? 255 : 0;
    return result;
}

// 二值膨胀
inline Image<1, uint8_t> binaryDilate(const Image<1, uint8_t>& image,
                                        const StructuringElement& se) {
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<1, uint8_t> result(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            bool hit = false;
            for (int si = 0; si < se.rows && !hit; ++si) {
                for (int sj = 0; sj < se.cols && !hit; ++sj) {
                    if (!se.data[si][sj]) continue;
                    int ni = i + si - se.origin_r;
                    int nj = j + sj - se.origin_c;
                    if (ni >= 0 && ni < rows && nj >= 0 && nj < cols
                        && image.at(ni, nj)[0] == 255) {
                        hit = true;
                    }
                }
            }
            result(i, j)[0] = hit ? 255 : 0;
        }
    }
    return result;
}

// 二值腐蚀
inline Image<1, uint8_t> binaryErode(const Image<1, uint8_t>& image,
                                       const StructuringElement& se) {
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<1, uint8_t> result(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            bool all_fit = true;
            for (int si = 0; si < se.rows && all_fit; ++si) {
                for (int sj = 0; sj < se.cols && all_fit; ++sj) {
                    if (!se.data[si][sj]) continue;
                    int ni = i + si - se.origin_r;
                    int nj = j + sj - se.origin_c;
                    if (ni < 0 || ni >= rows || nj < 0 || nj >= cols
                        || image.at(ni, nj)[0] != 255) {
                        all_fit = false;
                    }
                }
            }
            result(i, j)[0] = all_fit ? 255 : 0;
        }
    }
    return result;
}

// 二值开操作
inline Image<1, uint8_t> binaryOpen(const Image<1, uint8_t>& image,
                                      const StructuringElement& se) {
    return binaryDilate(binaryErode(image, se), se);
}

// 二值闭操作
inline Image<1, uint8_t> binaryClose(const Image<1, uint8_t>& image,
                                       const StructuringElement& se) {
    return binaryErode(binaryDilate(image, se), se);
}

// 取反
inline Image<1, uint8_t> binaryNot(const Image<1, uint8_t>& image) {
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<1, uint8_t> result(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result(i, j)[0] = (image.at(i, j)[0] == 255) ? 0 : 255;
    return result;
}

// 逐像素 AND
inline Image<1, uint8_t> binaryAnd(const Image<1, uint8_t>& a,
                                     const Image<1, uint8_t>& b) {
    int rows = static_cast<int>(a.rows());
    int cols = static_cast<int>(a.cols());
    Image<1, uint8_t> result(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result(i, j)[0] = (a.at(i, j)[0] == 255 && b.at(i, j)[0] == 255) ? 255 : 0;
    return result;
}

// ==================== 形态学重建（测地膨胀） ====================

inline Image<1, uint8_t> geodesicDilate(const Image<1, uint8_t>& marker,
                                          const Image<1, uint8_t>& mask,
                                          const StructuringElement& se) {
    auto dilated = binaryDilate(marker, se);
    return binaryAnd(dilated, mask);
}

inline Image<1, uint8_t> morphReconstruct(const Image<1, uint8_t>& marker,
                                            const Image<1, uint8_t>& mask,
                                            const StructuringElement& se,
                                            int max_iter = -1) {
    auto current = marker;
    int iter = 0;
    while (true) {
        auto next = geodesicDilate(current, mask, se);
        iter++;
        bool changed = false;
        for (int i = 0; i < static_cast<int>(current.rows()) && !changed; ++i)
            for (int j = 0; j < static_cast<int>(current.cols()) && !changed; ++j)
                if (current.at(i, j)[0] != next.at(i, j)[0])
                    changed = true;
        if (!changed) return next;
        current = next;
        if (max_iter > 0 && iter >= max_iter) return current;
    }
}

// ==================== 图9.29：长字符提取 ====================

inline Image<1, uint8_t> longCharacterExtraction(const Image<1, uint8_t>& binary,
                                                   int length = 51) {
    auto se = hlineSE(length);
    return binaryOpen(binary, se);
}

// ==================== 图9.31：空洞填充 ====================

inline Image<1, uint8_t> holeFilling(const Image<1, uint8_t>& binary) {
    // 在背景区域填充空洞：使用形态学重建
    int rows = static_cast<int>(binary.rows());
    int cols = static_cast<int>(binary.cols());

    // 取反：前景变背景
    auto complement = binaryNot(binary);

    // marker：边界像素（除了边界外全 0）
    Image<1, uint8_t> marker(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            marker(i, j)[0] = 0;
    // 边界像素设为 complement 值
    for (int i = 0; i < rows; ++i) {
        marker(i, 0)[0] = complement.at(i, 0)[0];
        marker(i, cols - 1)[0] = complement.at(i, cols - 1)[0];
    }
    for (int j = 0; j < cols; ++j) {
        marker(0, j)[0] = complement.at(0, j)[0];
        marker(rows - 1, j)[0] = complement.at(rows - 1, j)[0];
    }

    auto se = crossSE(3);
    auto filled_complement = morphReconstruct(marker, complement, se);
    return binaryNot(filled_complement);
}

// ==================== 图9.32：边界清除 ====================

inline Image<1, uint8_t> borderClearing(const Image<1, uint8_t>& binary) {
    int rows = static_cast<int>(binary.rows());
    int cols = static_cast<int>(binary.cols());

    // marker：只在边界上的前景像素
    Image<1, uint8_t> marker(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            marker(i, j)[0] = 0;
    for (int i = 0; i < rows; ++i) {
        marker(i, 0)[0] = binary.at(i, 0)[0];
        marker(i, cols - 1)[0] = binary.at(i, cols - 1)[0];
    }
    for (int j = 0; j < cols; ++j) {
        marker(0, j)[0] = binary.at(0, j)[0];
        marker(rows - 1, j)[0] = binary.at(rows - 1, j)[0];
    }

    auto se = crossSE(3);
    auto border_objects = morphReconstruct(marker, binary, se);

    // 原图 - 边界物体
    Image<1, uint8_t> result(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result(i, j)[0] = (binary.at(i, j)[0] == 255 && border_objects.at(i, j)[0] == 0) ? 255 : 0;
    return result;
}

// ==================== 灰度形态学 ====================

// 灰度膨胀（平坦 SE）
inline Image<1, uint8_t> grayDilate(const Image<1, uint8_t>& image,
                                      const StructuringElement& se) {
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<1, uint8_t> result(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            uint8_t max_val = 0;
            bool first = true;
            for (int si = 0; si < se.rows; ++si) {
                for (int sj = 0; sj < se.cols; ++sj) {
                    if (!se.data[si][sj]) continue;
                    int ni = i + si - se.origin_r;
                    int nj = j + sj - se.origin_c;
                    if (ni < 0 || ni >= rows || nj < 0 || nj >= cols) continue;
                    uint8_t v = image.at(ni, nj)[0];
                    if (first || v > max_val) { max_val = v; first = false; }
                }
            }
            result(i, j)[0] = first ? image.at(i, j)[0] : max_val;
        }
    }
    return result;
}

// 灰度腐蚀（平坦 SE）
inline Image<1, uint8_t> grayErode(const Image<1, uint8_t>& image,
                                     const StructuringElement& se) {
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<1, uint8_t> result(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            uint8_t min_val = 255;
            bool first = true;
            for (int si = 0; si < se.rows; ++si) {
                for (int sj = 0; sj < se.cols; ++sj) {
                    if (!se.data[si][sj]) continue;
                    int ni = i + si - se.origin_r;
                    int nj = j + sj - se.origin_c;
                    if (ni < 0 || ni >= rows || nj < 0 || nj >= cols) continue;
                    uint8_t v = image.at(ni, nj)[0];
                    if (first || v < min_val) { min_val = v; first = false; }
                }
            }
            result(i, j)[0] = first ? image.at(i, j)[0] : min_val;
        }
    }
    return result;
}

// 灰度开操作
inline Image<1, uint8_t> grayOpen(const Image<1, uint8_t>& image,
                                    const StructuringElement& se) {
    return grayDilate(grayErode(image, se), se);
}

// 灰度闭操作
inline Image<1, uint8_t> grayClose(const Image<1, uint8_t>& image,
                                     const StructuringElement& se) {
    return grayErode(grayDilate(image, se), se);
}

// ==================== 图9.40：顶帽变换 + 阈值分割 ====================

inline Image<1, uint8_t> topHatTransform(const Image<1, uint8_t>& image,
                                           const StructuringElement& se) {
    auto opened = grayOpen(image, se);
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<1, uint8_t> result(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result(i, j)[0] = static_cast<uint8_t>(
                std::clamp(static_cast<int>(image.at(i, j)[0]) - opened.at(i, j)[0] + 0, 0, 255));
    return result;
}

inline Image<1, uint8_t> topHatCorrect(const Image<1, uint8_t>& image,
                                         int se_size = 40) {
    // 使用大半径 disk SE 进行顶帽变换纠正阴影
    auto se = diskSE(se_size);
    return topHatTransform(image, se);
}

// ==================== 图9.41：粒度测定 ====================

inline std::vector<int> granulometry(const Image<1, uint8_t>& binary,
                                      int max_radius = 30) {
    std::vector<int> surface_areas;
    auto current = binary;
    int initial_area = 0;
    for (int i = 0; i < static_cast<int>(binary.rows()); ++i)
        for (int j = 0; j < static_cast<int>(binary.cols()); ++j)
            if (binary.at(i, j)[0] == 255) initial_area++;
    surface_areas.push_back(initial_area);

    for (int r = 1; r <= max_radius; ++r) {
        auto se = diskSE(r);
        auto opened = binaryOpen(current, se);
        int area = 0;
        for (int i = 0; i < static_cast<int>(opened.rows()); ++i)
            for (int j = 0; j < static_cast<int>(opened.cols()); ++j)
                if (opened.at(i, j)[0] == 255) area++;
        surface_areas.push_back(area);
        current = opened;
    }
    return surface_areas;
}

// 粒度测定可视化
inline Image<1, uint8_t> granulometryVisualize(const Image<1, uint8_t>& binary,
                                                 int max_radius = 30) {
    int rows = static_cast<int>(binary.rows());
    int cols = static_cast<int>(binary.cols());
    Image<1, uint8_t> result(rows, cols);

    auto areas = granulometry(binary, max_radius);

    // 使用开操作差异来着色
    auto current = binary;
    for (int r = 1; r <= max_radius; ++r) {
        auto se = diskSE(r);
        auto opened = binaryOpen(current, se);
        // 差异：被当前 SE 去除的粒子
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (current.at(i, j)[0] == 255 && opened.at(i, j)[0] == 0) {
                    uint8_t shade = static_cast<uint8_t>(std::min(255, r * 10));
                    result(i, j)[0] = shade;
                }
            }
        }
        current = opened;
    }
    // 保留未被任何 SE 去除的像素（最大粒度）
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (current.at(i, j)[0] == 255)
                result(i, j)[0] = 255;

    return result;
}

// ==================== 图9.43：分水岭分割预处理 ====================

// 形态学梯度
inline Image<1, uint8_t> morphGradient(const Image<1, uint8_t>& image,
                                         const StructuringElement& se) {
    auto dilated = grayDilate(image, se);
    auto eroded = grayErode(image, se);
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<1, uint8_t> result(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result(i, j)[0] = static_cast<uint8_t>(
                std::clamp(static_cast<int>(dilated.at(i, j)[0]) - eroded.at(i, j)[0], 0, 255));
    return result;
}

// 分水岭预处理：形态学平滑 + 梯度
inline Image<1, uint8_t> watershedPreprocess(const Image<1, uint8_t>& image,
                                               int smooth_radius = 3,
                                               int grad_radius = 3) {
    // 先平滑（开-闭级联）
    auto se_smooth = diskSE(smooth_radius);
    auto smoothed = grayClose(grayOpen(image, se_smooth), se_smooth);

    // 计算形态学梯度
    auto se_grad = diskSE(grad_radius);
    auto grad = morphGradient(smoothed, se_grad);

    return grad;
}

// 简化的分水岭标记提取（局部极小值 + 膨胀标记）
inline Image<1, uint8_t> watershedMarkers(const Image<1, uint8_t>& image,
                                            int min_distance = 10) {
    // 通过形态学开闭重建来平滑，然后提取前景标记
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());

    auto se_small = diskSE(3);
    auto se_large = diskSE(min_distance);

    // 开操作去除小物体
    auto opened = grayOpen(image, se_small);
    // 闭操作合并邻近区域
    auto closed = grayClose(opened, se_large);

    // 阈值：Otsu 的近似
    int hist[256] = {};
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            hist[closed.at(i, j)[0]]++;

    int total = rows * cols;
    float sum = 0;
    for (int i = 0; i < 256; ++i) sum += i * hist[i];
    float sumB = 0;
    int wB = 0;
    float max_var = 0;
    int threshold = 128;

    for (int t = 0; t < 256; ++t) {
        wB += hist[t];
        if (wB == 0) continue;
        int wF = total - wB;
        if (wF == 0) break;
        sumB += t * hist[t];
        float mB = sumB / wB;
        float mF = (sum - sumB) / wF;
        float var_between = static_cast<float>(wB) * static_cast<float>(wF) * (mB - mF) * (mB - mF);
        if (var_between > max_var) {
            max_var = var_between;
            threshold = t;
        }
    }

    Image<1, uint8_t> result(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result(i, j)[0] = (closed.at(i, j)[0] >= threshold) ? 255 : 0;
    return result;
}
