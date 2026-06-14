#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <cmath>

template <std::size_t N, typename T = uint8_t>
Image<N, T> nearestNeighborInterpolation(Image<N, T>* origin, float row_zoom, float col_zoom) {
    auto [originalRows, originalCols] = origin->getSize();
    int newRows = static_cast<int>(originalRows * row_zoom);
    int newCols = static_cast<int>(originalCols * col_zoom);
    auto processed = Image<N, T>(newRows, newCols);
    float originalX, originalY;
    for(int x = 0; x < newRows; ++x) {
        for(int y = 0; y < newCols; ++y) {
            originalX = x / row_zoom;
            originalY = y / col_zoom;
            int nearestX = static_cast<int>(originalX + 0.5f);
            int nearestY = static_cast<int>(originalY + 0.5f);
            processed(x, y) = origin->at_cut(nearestX, nearestY);
        }
    }
    return processed;
}


inline float cubic_weight(float t) {
    t = std::abs(t);
    float a = -0.5f;
    
    if (t < 1.0f) {
        return ((a + 2) * t - (a + 3)) * t * t + 1;
    } else if (t < 2.0f) {
        return ((a * t - 5 * a) * t + 8 * a) * t - 4 * a;
    }
    return 0.0f;
}
template <std::size_t N, typename T = uint8_t>
Image<N, T> bi_LinearInterpolation(Image<N, T>* origin, float row_zoom, float col_zoom) {
    auto [originalRows, originalCols] = origin->getSize();
    int newRows = static_cast<int>(originalRows * row_zoom);
    int newCols = static_cast<int>(originalCols * col_zoom);
    auto processed = Image<N, T>(newRows, newCols);

    float scaleX = static_cast<float>(originalCols) / newCols;
    float scaleY = static_cast<float>(originalRows) / newRows;
    float originalX, originalY;
    int x0, x1, y0, y1;

    for (int x = 0; x < newRows; ++x) {
        for (int y = 0; y < newCols; ++y) {
            float srcX = (x + 0.5f) * scaleY - 0.5f;
            float srcY = (y + 0.5f) * scaleX - 0.5f;
            
            int x0 = static_cast<int>(srcX);
            int y0 = static_cast<int>(srcY);
            int x1 = x0 + 1;
            int y1 = y0 + 1;
            
            float dx = srcX - x0;
            float dy = srcY - y0;
            
            // 读取原图并转换为 float 计算
            auto v00 = origin->at_cut(x0, y0);
            auto v01 = origin->at_cut(x0, y1);
            auto v10 = origin->at_cut(x1, y0);
            auto v11 = origin->at_cut(x1, y1);
            
            // 每个通道单独计算，直接使用 float
            Pixel<T, N> result;
            for (std::size_t c = 0; c < N; ++c) {
                float f00 = static_cast<float>(v00[c]);
                float f01 = static_cast<float>(v01[c]);
                float f10 = static_cast<float>(v10[c]);
                float f11 = static_cast<float>(v11[c]);
                
                float top = f00 * (1 - dy) + f01 * dy;
                float bottom = f10 * (1 - dy) + f11 * dy;
                float val = top * (1 - dx) + bottom * dx;
                
                // clamp 并转换回 T
                val = std::clamp(val, 0.0f, 255.0f);
                result[c] = static_cast<T>(val + 0.5f);
            }
            
            processed(x, y) = result;
        }
    }
    return processed;
}

template <std::size_t N, typename T>
Image<N, T> bi_CubicInterpolation(Image<N, T>* origin, float row_zoom, float col_zoom) {
    auto [originalRows, originalCols] = origin->getSize();
    int newRows = static_cast<int>(originalRows * row_zoom);
    int newCols = static_cast<int>(originalCols * col_zoom);
    auto processed = Image<N, T>(newRows, newCols);
    
    float scaleY = static_cast<float>(originalRows) / newRows;
    float scaleX = static_cast<float>(originalCols) / newCols;
    
    for (int x = 0; x < newRows; ++x) {
        for (int y = 0; y < newCols; ++y) {
            float srcX = (x + 0.5f) * scaleY - 0.5f;
            float srcY = (y + 0.5f) * scaleX - 0.5f;
            
            int x0 = static_cast<int>(std::floor(srcX));
            int y0 = static_cast<int>(std::floor(srcY));
            
            float dx = srcX - x0;
            float dy = srcY - y0;
            
            typename Image<N, T>::pixel_type result;
            
            // 对每个通道单独计算
            for (std::size_t c = 0; c < N; ++c) {
                float sum = 0.0f;
                
                // 遍历 4x4 = 16 个邻域像素
                for (int i = -1; i <= 2; ++i) {
                    for (int j = -1; j <= 2; ++j) {
                        int px = x0 + i;
                        int py = y0 + j;
                        
                        float weight_x = cubic_weight(dx - i);
                        float weight_y = cubic_weight(dy - j);
                        float weight = weight_x * weight_y;
                        
                        float pixel_value = static_cast<float>(origin->at_cut(px, py)[c]);
                        sum += pixel_value * weight;
                    }
                }
                
                // 对于 uint8_t 类型，clamp 到 [0, 255]
                if constexpr (std::is_same_v<T, uint8_t>) {
                    sum = std::clamp(sum, 0.0f, 255.0f);
                    result[c] = static_cast<T>(sum + 0.5f);
                } else {
                    result[c] = static_cast<T>(sum);
                }
            }
            
            processed(x, y) = result;
        }
    }
    return processed;
}
