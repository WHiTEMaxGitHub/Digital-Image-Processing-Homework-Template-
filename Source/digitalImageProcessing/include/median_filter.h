#pragma once
#include "image.h"
#include "histogram.h"  // for imageEqual
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>

// 中值滤波（单次）
// kernel_size: 滤波核大小（奇数，如 3, 5, 7...）
template<std::size_t N, typename T = uint8_t>
Image<N, T> medianFilter(const Image<N, T>& image, int kernel_size = 3) {
    static_assert(std::is_same_v<T, uint8_t>, "Only supports uint8_t type");
    
    if (kernel_size % 2 == 0) {
        throw std::runtime_error("Median filter kernel size must be odd");
    }
    
    int half = kernel_size / 2;
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    int total = kernel_size * kernel_size;
    int mid = total / 2;
    
    Image<N, T> result(rows, cols);
    
    // 为每个通道分配排序缓冲区
    std::vector<T> buffer(total);
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Pixel<N, T> pixel;
            
            for (std::size_t c = 0; c < N; ++c) {
                // 收集邻域像素值
                int idx = 0;
                for (int di = -half; di <= half; ++di) {
                    for (int dj = -half; dj <= half; ++dj) {
                        int ni = i + di;
                        int nj = j + dj;
                        buffer[idx++] = image.at_cut(ni, nj)[c];
                    }
                }
                
                // 排序取中值
                std::nth_element(buffer.begin(), buffer.begin() + mid, buffer.end());
                pixel[c] = buffer[mid];
            }
            
            result(i, j) = pixel;
        }
    }
    
    return result;
}

// 中值滤波迭代版本（指定次数，不检查收敛）
// n: 迭代次数
// kernel_size: 滤波核大小
template<std::size_t N, typename T = uint8_t>
Image<N, T> medianFilterN(const Image<N, T>& image, int n, int kernel_size = 3) {
    static_assert(std::is_same_v<T, uint8_t>, "Only supports uint8_t type");
    
    Image<N, T> result = image;
    for (int i = 0; i < n; ++i) {
        result = medianFilter(result, kernel_size);
    }
    return result;
}

// 中值滤波迭代到收敛版本
// max_iterations: 最大迭代次数（-1 表示无上限，直至收敛）
// kernel_size: 滤波核大小
// 返回 pair<最终图像, 实际迭代次数>
template<std::size_t N, typename T = uint8_t>
std::pair<Image<N, T>, int> medianFilterIterative(
    const Image<N, T>& image, 
    int max_iterations = -1,
    int kernel_size = 3) 
{
    static_assert(std::is_same_v<T, uint8_t>, "Only supports uint8_t type");
    
    Image<N, T> current = image;
    int iter = 0;
    
    while (true) {
        Image<N, T> next = medianFilter(current, kernel_size);
        iter++;
        
        // 检查是否收敛
        if (imageEqual(current, next)) {
            return {next, iter};
        }
        
        current = next;
        
        if (max_iterations > 0 && iter >= max_iterations) {
            return {current, iter};
        }
    }
}
