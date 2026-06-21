#pragma once

#include "histogram.h"
#include "image.h"
#include "interpolators.h"
#include "median_filter.h"
#include "laplacian.h"

#include <filesystem>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

// 全局数据（原在 main.cpp 中）
inline float zoom_num[] = {2.0f, 0.5f};
inline std::string interpolator_names[][2] = {
    {"Nearest Neighbor Interpolation", "NNI"},
    {"Bi-linear Interpolation",        "BLI"},
    {"Bi-cubic Interpolation",         "BCI"}
};

// ==================== 任务1：插值 ====================

inline bool interpolation_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path)) {
        throw std::runtime_error("文件夹不存在或路径无效");
    }
    auto output_dir_path = path / "../output";
    output_dir_path = output_dir_path.lexically_normal();
    if (!fs::exists(output_dir_path)) {
        fs::create_directory(output_dir_path);
    }

    using InterpFunc = Image<1, uint8_t> (*)(Image<1, uint8_t>*, float, float);
    InterpFunc interpolators[] = {nearestNeighborInterpolation<1, uint8_t>,
                                  bi_LinearInterpolation<1, uint8_t>,
                                  bi_CubicInterpolation<1, uint8_t>};

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            float zoom = zoom_num[j];
            for (const auto& entry: fs::directory_iterator(path)) {
                auto image = Image<1, uint8_t>::createImage(entry.path());
                image.setInterpolator(interpolators[i]);
                auto output_image_path = output_dir_path /
                                         (entry.path().stem().string() +
                                          "_processed_" +
                                          interpolator_names[i][1] + "_" +
                                          std::to_string(zoom) +
                                          entry.path().extension().string());
                image.interpolate(zoom, zoom).save(output_image_path);
            }
        }
        std::cout << interpolator_names[i][0] << " Finished." << std::endl;
    }
    return true;
}

// ==================== 任务2-1：迭代直方图均衡化 ====================

inline bool histogramEqualization_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path)) {
        throw std::runtime_error("文件夹不存在或路径无效");
    }
    auto output_dir_path = path / "../output";
    output_dir_path = output_dir_path.lexically_normal();
    if (!fs::exists(output_dir_path)) {
        fs::create_directory(output_dir_path);
    }

    int iterations[] = {1, 3, 5, 10};

    for (const auto& entry: fs::directory_iterator(path)) {
        auto image = Image<1, uint8_t>::createImage(entry.path());
        std::string stem = entry.path().stem().string();
        std::string ext = entry.path().extension().string();

        for (int n : iterations) {
            auto result = histogramEqualizationN(image, n);
            auto output_image_path = output_dir_path /
                                     (stem + "_processed_HE_n" +
                                      std::to_string(n) + ext);
            result.save(output_image_path);
            std::cout << "  HE n=" << n << " -> "
                      << output_image_path.filename().string() << std::endl;
        }

        auto [result_inf, actual_iter] = histogramEqualizationIterative(image);
        auto output_image_path = output_dir_path /
                                 (stem + "_processed_HE_inf_" +
                                  std::to_string(actual_iter) + "iter" + ext);
        result_inf.save(output_image_path);
        std::cout << "  HE inf (converged at iter=" << actual_iter
                  << ") -> " << output_image_path.filename().string() << std::endl;
    }

    std::cout << "Histogram Equalization (Iterative) Finished." << std::endl;
    return true;
}

// ==================== 任务2-2：中值滤波 ====================

inline bool medianFilter_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path)) {
        throw std::runtime_error("文件夹不存在或路径无效");
    }
    auto output_dir_path = path / "../output";
    output_dir_path = output_dir_path.lexically_normal();
    if (!fs::exists(output_dir_path)) {
        fs::create_directory(output_dir_path);
    }

    int iterations[] = {1, 3, 5, 10};
    int kernel_size = 3;

    for (const auto& entry: fs::directory_iterator(path)) {
        auto image = Image<1, uint8_t>::createImage(entry.path());
        std::string stem = entry.path().stem().string();
        std::string ext = entry.path().extension().string();

        for (int n : iterations) {
            auto result = medianFilterN(image, n, kernel_size);
            auto output_image_path = output_dir_path /
                                     (stem + "_processed_MF_n" +
                                      std::to_string(n) + ext);
            result.save(output_image_path);
            std::cout << "  MF (k=" << kernel_size << ") n=" << n << " -> "
                      << output_image_path.filename().string() << std::endl;
        }

        auto [result_inf, actual_iter] = medianFilterIterative(image, -1, kernel_size);
        auto output_image_path = output_dir_path /
                                 (stem + "_processed_MF_inf_" +
                                  std::to_string(actual_iter) + "iter" + ext);
        result_inf.save(output_image_path);
        std::cout << "  MF (k=" << kernel_size << ") inf (converged at iter="
                  << actual_iter << ") -> "
                  << output_image_path.filename().string() << std::endl;
    }

    std::cout << "Median Filter Finished." << std::endl;
    return true;
}

// ==================== 任务2-3：拉普拉斯锐化 ====================

inline bool laplacianSharpen_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path)) {
        throw std::runtime_error("文件夹不存在或路径无效");
    }
    auto output_dir_path = path / "../output";
    output_dir_path = output_dir_path.lexically_normal();
    if (!fs::exists(output_dir_path)) {
        fs::create_directory(output_dir_path);
    }

    LaplacianType types[] = {LaplacianType::FOUR_NEIGHBOR, LaplacianType::EIGHT_NEIGHBOR};
    std::string type_names[] = {"L4", "L8"};

    float strengths[] = {0.5f, 1.0f, 1.5f, 2.0f};

    for (const auto& entry: fs::directory_iterator(path)) {
        auto image = Image<1, uint8_t>::createImage(entry.path());
        std::string stem = entry.path().stem().string();
        std::string ext = entry.path().extension().string();

        for (int t = 0; t < 2; ++t) {
            for (float s : strengths) {
                auto result = laplacianSharpen(image, types[t], s);
                auto output_image_path = output_dir_path /
                                         (stem + "_processed_LS_" +
                                          type_names[t] + "_s" +
                                          std::to_string(s) + ext);
                result.save(output_image_path);
                std::cout << "  LS " << type_names[t] << " strength=" << s
                          << " -> " << output_image_path.filename().string() << std::endl;
            }
        }
    }

    std::cout << "Laplacian Sharpen Finished." << std::endl;
    return true;
}
