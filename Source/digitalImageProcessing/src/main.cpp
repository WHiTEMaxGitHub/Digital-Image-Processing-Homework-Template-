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
template<std::size_t N, typename T = uint8_t>
std::function<Image<N, T>(Image<N, T>*, float, float)> interpolators[] = {
    nearestNeighborInterpolation<N, T>, bi_LinearInterpolation<N, T>,
    bi_CubicInterpolation<N, T>};

bool interpolation_test(fs::path path) {
    path = fs::absolute(path);
    if (!fs::exists(path) || !fs::is_directory(path)) {
        throw std::runtime_error("文件夹不存在或路径无效");
    }
    auto output_dir_path = path / "../output";
    output_dir_path = output_dir_path.lexically_normal();
    if(!fs::exists(output_dir_path)) {
        fs::create_directory(output_dir_path);
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            float zoom = zoom_num[j];
            for (const auto& entry: fs::directory_iterator(path)) {
                auto image = Image<1>::createImage(entry.path());
                image.setInterpolator(interpolators<1>[i]);
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
                    std::cout << "test failed" << std::endl;
                } else {
                    std::cout << "test succeeded" << std::endl;
                }
                break;
            default:
                std::cout << "参数 " << argv[2]
                          << " 错误, 应当为[1, 4]中的某个数字." << std::endl;
                return 1;
        }
    }
    return 0;
}
