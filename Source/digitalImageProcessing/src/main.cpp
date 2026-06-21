#include "tests.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "参数应该为3, 形式为: " << std::endl;
        std::cout << argv[0] << " <作业编号> <输入图片文件夹路径>."
                  << std::endl;
        std::cout << "作业编号: 1, 2-1(直方图均衡化), 2-2(中值滤波), 2-3(拉普拉斯锐化),"
                  << " 3-1(金字塔), 3-2(小波变换), 3-3(小波边缘检测),"
                  << " 4-1(二值形态学), 4-2(灰度形态学)"
                  << std::endl;
        return 1;
    }

    std::string mod_str = std::string(argv[1]);

    if (mod_str == "1") {
        if (!interpolation_test(argv[2])) {
            std::cout << "test 1 failed" << std::endl;
        } else {
            std::cout << "test 1 succeeded" << std::endl;
        }
    }
    else if (mod_str.length() == 3 && mod_str[1] == '-') {
        char sub = mod_str[2];
        char major = mod_str[0];
        switch (sub) {
            case '1':
                if (major == '2') {
                    if (histogramEqualization_test(argv[2]))
                        std::cout << "test 2-1 succeeded" << std::endl;
                    else
                        std::cout << "test 2-1 failed" << std::endl;
                } else if (major == '3') {
                    if (pyramid_test(argv[2]))
                        std::cout << "test 3-1 succeeded" << std::endl;
                    else
                        std::cout << "test 3-1 failed" << std::endl;
                } else if (major == '4') {
                    if (binaryMorphology_test(argv[2]))
                        std::cout << "test 4-1 succeeded" << std::endl;
                    else
                        std::cout << "test 4-1 failed" << std::endl;
                }
                break;
            case '2':
                if (major == '2') {
                    if (medianFilter_test(argv[2]))
                        std::cout << "test 2-2 succeeded" << std::endl;
                    else
                        std::cout << "test 2-2 failed" << std::endl;
                } else if (major == '3') {
                    if (waveletTransform_test(argv[2]))
                        std::cout << "test 3-2 succeeded" << std::endl;
                    else
                        std::cout << "test 3-2 failed" << std::endl;
                } else if (major == '4') {
                    if (grayMorphology_test(argv[2]))
                        std::cout << "test 4-2 succeeded" << std::endl;
                    else
                        std::cout << "test 4-2 failed" << std::endl;
                }
                break;
            case '3':
                if (major == '2') {
                    if (laplacianSharpen_test(argv[2]))
                        std::cout << "test 2-3 succeeded" << std::endl;
                    else
                        std::cout << "test 2-3 failed" << std::endl;
                } else if (major == '3') {
                    if (waveletEdgeDetection_test(argv[2]))
                        std::cout << "test 3-3 succeeded" << std::endl;
                    else
                        std::cout << "test 3-3 failed" << std::endl;
                }
                break;
            default:
                std::cout << "作业的子编号应当为'1', '2'或'3'"
                          << std::endl;
                return 1;
        }
    }
    else {
        std::cout << "参数 " << argv[1]
                  << " 错误, 模式应当为'1'或'2-{1,2,3}'/'3-{1,2,3}'/'4-{1,2}'."
                  << std::endl;
        return 1;
    }
    return 0;
}
