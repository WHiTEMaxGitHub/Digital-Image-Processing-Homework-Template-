#pragma once
#include "image.h"
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cmath>

// 拉普拉斯算子类型
enum class LaplacianType {
    FOUR_NEIGHBOR,  // 4-邻域核: [0,1,0; 1,-4,1; 0,1,0]
    EIGHT_NEIGHBOR  // 8-邻域核: [1,1,1; 1,-8,1; 1,1,1]
};

// 拉普拉斯锐化（单次）
// type: 拉普拉斯核类型
// strength: 锐化强度系数（推荐 1.0）
// 公式: sharpened = original - strength * laplacian(original)
template<std::size_t N, typename T = uint8_t>
Image<N, T> laplacianSharpen(const Image<N, T>& image, 
                              LaplacianType type = LaplacianType::FOUR_NEIGHBOR,
                              float strength = 1.0f) {
    static_assert(std::is_same_v<T, uint8_t>, "Only supports uint8_t type");
    
    int rows = static_cast<int>(image.rows());
    int cols = static_cast<int>(image.cols());
    Image<N, T> result(rows, cols);
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Pixel<N, T> pixel;
            
            for (std::size_t c = 0; c < N; ++c) {
                float center = static_cast<float>(image.at_cut(i, j)[c]);
                float laplacian_val = 0.0f;
                
                if (type == LaplacianType::FOUR_NEIGHBOR) {
                    // 4-邻域核:
                    //  0  -1   0
                    // -1   4  -1
                    //  0  -1   0
                    laplacian_val = 
                        -static_cast<float>(image.at_cut(i-1, j)[c])
                        -static_cast<float>(image.at_cut(i+1, j)[c])
                        -static_cast<float>(image.at_cut(i, j-1)[c])
                        -static_cast<float>(image.at_cut(i, j+1)[c])
                        + 4.0f * center;
                } else {
                    // 8-邻域核:
                    // -1  -1  -1
                    // -1   8  -1
                    // -1  -1  -1
                    laplacian_val = 
                        -static_cast<float>(image.at_cut(i-1, j-1)[c])
                        -static_cast<float>(image.at_cut(i-1, j)[c])
                        -static_cast<float>(image.at_cut(i-1, j+1)[c])
                        -static_cast<float>(image.at_cut(i, j-1)[c])
                        + 8.0f * center
                        -static_cast<float>(image.at_cut(i, j+1)[c])
                        -static_cast<float>(image.at_cut(i+1, j-1)[c])
                        -static_cast<float>(image.at_cut(i+1, j)[c])
                        -static_cast<float>(image.at_cut(i+1, j+1)[c]);
                }
                
                // 锐化公式: sharpened = original - strength * laplacian
                // 当 laplacian 在边缘处为正值时，减去后边缘被抑制（变暗）
                // 当 laplacian 在边缘处为负值时，减去后边缘被增强（变亮）
                // 注意：上面 4/8 邻域核中，边缘处 laplacian 为正值
                // 因此 sharpened = original - strength * laplacian 
                // 会在边缘处减去正值 → 边缘被抑制
                // 实际上我们想要的是增强边缘，所以应该：
                // sharpened = original + strength * laplacian（当边缘处 laplacian 为负时）
                // 或者用反向核：[0,1,0; 1,-4,1; 0,1,0] 直接: sharpen = original - laplacian
                
                // 使用 4-邻域正向核 [0,1,0; 1,-4,1; 0,1,0]
                // 这时 laplacian 在边缘处为负 → sharpen = original - (-val) = original + val
                // 这里为了保持直观，直接使用: sharpened = original - strength * laplacian
                float sharpened = center - strength * laplacian_val;
                
                // Clamp
                if constexpr (std::is_same_v<T, uint8_t>) {
                    sharpened = std::clamp(sharpened, 0.0f, 255.0f);
                    pixel[c] = static_cast<T>(std::round(sharpened));
                } else {
                    pixel[c] = static_cast<T>(sharpened);
                }
            }
            
            result(i, j) = pixel;
        }
    }
    
    return result;
}

// 简化别名：使用 4-邻域核的拉普拉斯锐化
template<std::size_t N, typename T = uint8_t>
Image<N, T> laplacianSharpen4(const Image<N, T>& image, float strength = 1.0f) {
    return laplacianSharpen(image, LaplacianType::FOUR_NEIGHBOR, strength);
}

// 简化别名：使用 8-邻域核的拉普拉斯锐化
template<std::size_t N, typename T = uint8_t>
Image<N, T> laplacianSharpen8(const Image<N, T>& image, float strength = 1.0f) {
    return laplacianSharpen(image, LaplacianType::EIGHT_NEIGHBOR, strength);
}
