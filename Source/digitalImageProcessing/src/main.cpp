#include "histogram.h"
#include "image.h"
#include "interpolators.h"

#include <filesystem>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

float zoom_num[] = {2.0f, 0.5f};
std::string interpolator_names[][2] = {"Nearest Neighbor Interpolation", "NNI",
                                       "Bi-linear Interpolation",        "BLI",
                                       "Bi-cubic Interpolation",         "BCI"};

// 注意：这里需要为每个具体的 N 和 T 实例化，但函数指针不能直接存储模板函数
// 改用 lambda 包装或直接在使用时传递函数指针

bool interpolation_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path)) {
        throw std::runtime_error("文件夹不存在或路径无效");
    }
    auto output_dir_path = path / "../output";
    output_dir_path = output_dir_path.lexically_normal();
    if (!fs::exists(output_dir_path)) {
        fs::create_directory(output_dir_path);
    }

    // 使用函数指针数组，注意模板函数需要实例化
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

bool histogramEqulization_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path)) {
        throw std::runtime_error("文件夹不存在或路径无效");
    }
    auto output_dir_path = path / "../output";
    output_dir_path = output_dir_path.lexically_normal();
    if (!fs::exists(output_dir_path)) {
        fs::create_directory(output_dir_path);
    }
    for (const auto& entry: fs::directory_iterator(path)) {
        auto image = Image<1, uint8_t>::createImage(entry.path());
        auto output_image_path = output_dir_path /
                                 (entry.path().stem().string() + "_processed" +
                                  entry.path().extension().string());
        histogramEqualization(image).save(output_image_path);
    }
    return true;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "参数应该为3, 形式为: " << std::endl;
        std::cout << argv[0] << " <作业编号> <输入图片文件夹路径>."
                  << std::endl;
        return 1;
    } else {
        int mod = argv[1][0] - '0';
        switch (mod) {
            case 1:
                if (!interpolation_test(argv[2])) {
                    std::cout << "test 1 failed" << std::endl;
                } else {
                    std::cout << "test 1 succeeded" << std::endl;
                }
                break;
            case 2: {
                auto s = std::string(argv[1]);
                if (s.length() != 3 || s[1] != '-' ||
                    !(s[2] == '1' || s[2] == '2')) {
                    std::cout << "作业2的输入应该为'2-1'或'2-2'" << std::endl;
                } else {
                    if (s[2] == '1') {
                        if (histogramEqulization_test(argv[2])) {
                            std::cout << "test 2-1 succeeded" << std::endl;
                        } else {
                            std::cout << "test 2-1 failed" << std::endl;
                        }
                    }
                }
                break;
            }
            default:
                std::cout << "参数 " << argv[1]
                          << " 错误, 模式应当为[1, 4]中的某个数字."
                          << std::endl;
                return 1;
        }
    }
    return 0;
}