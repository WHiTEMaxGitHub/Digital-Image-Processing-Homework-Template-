#include "tests.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "参数应该为3, 形式为: " << std::endl;
        std::cout << argv[0] << " <作业编号> <输入图片文件夹路径>."
                  << std::endl;
        std::cout << "作业编号: 1, 2-1(直方图均衡化), 2-2(中值滤波), 2-3(拉普拉斯锐化)"
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
    else if (mod_str.length() == 3 && mod_str[0] == '2' && mod_str[1] == '-') {
        char sub = mod_str[2];
        switch (sub) {
            case '1':
                if (histogramEqualization_test(argv[2])) {
                    std::cout << "test 2-1 succeeded" << std::endl;
                } else {
                    std::cout << "test 2-1 failed" << std::endl;
                }
                break;
            case '2':
                if (medianFilter_test(argv[2])) {
                    std::cout << "test 2-2 succeeded" << std::endl;
                } else {
                    std::cout << "test 2-2 failed" << std::endl;
                }
                break;
            case '3':
                if (laplacianSharpen_test(argv[2])) {
                    std::cout << "test 2-3 succeeded" << std::endl;
                } else {
                    std::cout << "test 2-3 failed" << std::endl;
                }
                break;
            default:
                std::cout << "作业2的子编号应当为'1', '2'或'3'"
                          << std::endl;
                return 1;
        }
    }
    else {
        std::cout << "参数 " << argv[1]
                  << " 错误, 模式应当为'1'或'2-1'/'2-2'/'2-3'."
                  << std::endl;
        return 1;
    }
    return 0;
}
