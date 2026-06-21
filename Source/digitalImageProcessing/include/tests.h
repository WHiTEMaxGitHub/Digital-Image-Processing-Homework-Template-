#pragma once

#include "histogram.h"
#include "image.h"
#include "interpolators.h"
#include "median_filter.h"
#include "laplacian.h"
#include "pyramid.h"
#include "wavelet.h"
#include "morphology.h"

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

// ==================== 任务3-1：金字塔 ====================

inline bool pyramid_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path))
        throw std::runtime_error("文件夹不存在或路径无效");
    auto output_dir = path / "../output";
    output_dir = output_dir.lexically_normal();
    if (!fs::exists(output_dir)) fs::create_directory(output_dir);

    int levels = 4; // 金字塔级数

    for (const auto& entry : fs::directory_iterator(path)) {
        auto image = Image<1, uint8_t>::createImage(entry.path());
        std::string stem = entry.path().stem().string();
        std::string ext = entry.path().extension().string();

        // 近似金字塔
        auto approx = buildApproximationPyramid(image, levels);
        for (int l = 0; l <= levels; ++l) {
            auto out_path = output_dir / (stem + "_approx_lv" + std::to_string(l) + ext);
            approx[l].save(out_path);
            std::cout << "  Approx level " << l << " (" << approx[l].rows()
                      << "x" << approx[l].cols() << ") -> "
                      << out_path.filename().string() << std::endl;
        }

        // 预测残差金字塔
        auto residual = buildResidualPyramid(image, levels);
        for (int l = 0; l <= levels; ++l) {
            auto out_path = output_dir / (stem + "_residual_lv" + std::to_string(l) + ext);
            residual[l].save(out_path);
            std::cout << "  Residual level " << l << " (" << residual[l].rows()
                      << "x" << residual[l].cols() << ") -> "
                      << out_path.filename().string() << std::endl;
        }
    }

    std::cout << "Pyramid (Approximation + Residual) Finished." << std::endl;
    return true;
}

// ==================== 任务3-2：二维快速小波变换 ====================

inline bool waveletTransform_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path))
        throw std::runtime_error("文件夹不存在或路径无效");
    auto output_dir = path / "../output";
    output_dir = output_dir.lexically_normal();
    if (!fs::exists(output_dir)) fs::create_directory(output_dir);

    auto wf = db4Filter();
    int levels = 3;

    for (const auto& entry : fs::directory_iterator(path)) {
        auto image = Image<1, uint8_t>::createImage(entry.path());
        std::string stem = entry.path().stem().string();
        std::string ext = entry.path().extension().string();

        // 单级分解（2×2 拼接图）
        auto result_1 = fwt2d_multilevel(image, wf, 1);
        auto out_path_1 = output_dir / (stem + "_fwt_lv1" + ext);
        result_1.save(out_path_1);
        std::cout << "  FWT level 1 -> "
                  << out_path_1.filename().string() << std::endl;

        // 多级分解
        auto result_n = fwt2d_multilevel(image, wf, levels);
        auto out_path_n = output_dir / (stem + "_fwt_lv" + std::to_string(levels) + ext);
        result_n.save(out_path_n);
        std::cout << "  FWT level " << levels << " -> "
                  << out_path_n.filename().string() << std::endl;
    }

    std::cout << "2D Fast Wavelet Transform Finished." << std::endl;
    return true;
}

// ==================== 任务3-3：小波边缘检测 ====================

inline bool waveletEdgeDetection_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path))
        throw std::runtime_error("文件夹不存在或路径无效");
    auto output_dir = path / "../edge_output";
    output_dir = output_dir.lexically_normal();
    if (!fs::exists(output_dir)) fs::create_directory(output_dir);

    auto wf = db4Filter();

    for (const auto& entry : fs::directory_iterator(path)) {
        auto image = Image<1, uint8_t>::createImage(entry.path());
        std::string stem = entry.path().stem().string();
        std::string ext = entry.path().extension().string();

        for (float th : {1.0f, 1.5f, 2.0f}) {
            auto edge = waveletEdgeDetection(image, wf, 1, th);
            char buf[16];
            std::snprintf(buf, sizeof(buf), "%.1f", th);
            auto out_path = output_dir / (stem + "_edge_th" + buf + ext);
            edge.save(out_path);
            std::cout << "  Edge th=" << th << " -> "
                      << out_path.filename().string() << std::endl;
        }
    }

    std::cout << "Wavelet Edge Detection Finished." << std::endl;
    return true;
}

// ==================== 任务4-1：二值形态学 ====================

inline bool binaryMorphology_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path))
        throw std::runtime_error("文件夹不存在或路径无效");
    auto output_dir = path / "../output";
    output_dir = output_dir.lexically_normal();
    if (!fs::exists(output_dir)) fs::create_directory(output_dir);

    for (const auto& entry : fs::directory_iterator(path)) {
        auto gray = Image<1, uint8_t>::createImage(entry.path());
        auto binary = threshold(gray, 128);
        std::string stem = entry.path().stem().string();
        std::string ext = entry.path().extension().string();

        // 图9.29：长字符提取
        auto long_char = longCharacterExtraction(binary, 51);
        auto p1 = output_dir / (stem + "_long_char" + ext);
        long_char.save(p1);
        std::cout << "  Long char extraction -> " << p1.filename().string() << std::endl;

        // 图9.31：空洞填充
        auto filled = holeFilling(binary);
        auto p2 = output_dir / (stem + "_hole_fill" + ext);
        filled.save(p2);
        std::cout << "  Hole filling -> " << p2.filename().string() << std::endl;

        // 图9.32：边界清除
        auto cleared = borderClearing(binary);
        auto p3 = output_dir / (stem + "_border_clear" + ext);
        cleared.save(p3);
        std::cout << "  Border clearing -> " << p3.filename().string() << std::endl;
    }

    std::cout << "Binary Morphology Finished." << std::endl;
    return true;
}

// ==================== 任务4-2：灰度形态学 ====================

inline bool grayMorphology_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path))
        throw std::runtime_error("文件夹不存在或路径无效");
    auto output_dir = path / "../output";
    output_dir = output_dir.lexically_normal();
    if (!fs::exists(output_dir)) fs::create_directory(output_dir);

    for (const auto& entry : fs::directory_iterator(path)) {
        auto image = Image<1, uint8_t>::createImage(entry.path());
        std::string stem = entry.path().stem().string();
        std::string base = stem;
        // 截取 FigXXXX(a) 中的编号
        auto pos = stem.find("Fig");
        if (pos != std::string::npos) {
            auto end = stem.find("(a)");
            if (end == std::string::npos) end = stem.find("_");
            if (end != std::string::npos)
                base = stem.substr(pos, end - pos);
        }
        std::string ext = entry.path().extension().string();

        if (stem.find("0940") != std::string::npos) {
            // 图9.40：顶帽变换纠正阴影
            auto tophat = topHatCorrect(image, 40);
            auto p = output_dir / (base + "_tophat" + ext);
            tophat.save(p);
            std::cout << "  Top-hat -> " << p.filename().string() << std::endl;

            // Otsu 阈值分割
            auto binary = watershedMarkers(tophat, 10);
            auto pb = output_dir / (base + "_tophat_binary" + ext);
            binary.save(pb);
            std::cout << "  Top-hat + threshold -> " << pb.filename().string() << std::endl;
        }

        if (stem.find("0941") != std::string::npos) {
            // 图9.41：粒度测定
            auto binary = threshold(image, 128);
            auto granulo = granulometryVisualize(binary, 30);
            auto p = output_dir / (base + "_granulometry" + ext);
            granulo.save(p);
            std::cout << "  Granulometry -> " << p.filename().string() << std::endl;
        }

        if (stem.find("0943") != std::string::npos) {
            // 图9.43：分水岭分割预处理
            auto grad = watershedPreprocess(image, 3, 3);
            auto p = output_dir / (base + "_watershed_grad" + ext);
            grad.save(p);
            std::cout << "  Watershed gradient -> " << p.filename().string() << std::endl;

            auto markers = watershedMarkers(image, 10);
            auto pm = output_dir / (base + "_watershed_markers" + ext);
            markers.save(pm);
            std::cout << "  Watershed markers -> " << pm.filename().string() << std::endl;
        }
    }

    std::cout << "Gray Morphology Finished." << std::endl;
    return true;
}