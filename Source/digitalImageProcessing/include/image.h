#pragma once
#include "pixel.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

template<std::size_t N, typename T = uint8_t>
class Image {
public:
    using pixel_type = Pixel<N, T>;
    inline static constexpr std::size_t channel_n = N;
    Image(): _rows(0), _cols(0) {}
    Image(int rows, int cols): _rows(rows), _cols(cols) {
        _data = std::vector<pixel_type>(_rows * _cols);
    }
    static Image createImage(fs::path image_path);

    // x = row (向下), y = col (向右)
    pixel_type& operator()(std::size_t x, std::size_t y);
    pixel_type& operator()(std::pair<std::size_t, std::size_t> coord);
    pixel_type at(std::size_t x, std::size_t y) const;
    pixel_type at(std::pair<std::size_t, std::size_t> coord) const;
    pixel_type at_cut(int x, int y) const;
    pixel_type at_cut(std::pair<int, int> coord) const;

    bool save(const fs::path& image_path) const;

    inline std::size_t rows() const { return _rows; }
    inline std::size_t cols() const { return _cols; }
    inline std::pair<std::size_t, std::size_t> getSize() const {
        return std::make_pair(_rows, _cols);
    }

    inline void setInterpolator(
        std::function<Image<N, T>(Image<N, T>*, float, float)> interpolator) {
        _interpolator = interpolator;
        return;
    }
    inline Image<N, T> interpolate(float row_zoom, float col_zoom) {
        if (_interpolator)
            return _interpolator(this, row_zoom, col_zoom);
        else
            throw std::runtime_error("No interpolation function found");
    }

private:
    cv::Mat toMat() const;
    std::vector<pixel_type> _data;
    std::size_t _rows, _cols; // rows = 行数(高), cols = 列数(宽)
    std::function<Image<N, T>(Image<N, T>*, float, float)> _interpolator;
};

// 创建Image类的工厂方法
template<std::size_t N, typename T>
Image<N, T> Image<N, T>::createImage(fs::path image_path) {
    cv::Mat mat;
    if constexpr (N == 1) {
        mat = cv::imread(image_path.string(), cv::IMREAD_GRAYSCALE);
    } else if constexpr (N == 3) {
        mat = cv::imread(image_path.string(), cv::IMREAD_COLOR);
    } else {
        mat = cv::imread(image_path.string(), cv::IMREAD_UNCHANGED);
    }
    if (mat.empty()) {
        throw std::runtime_error("Failed to load: " + image_path.string());
    }

    Image<N, T> img;
    img._rows = mat.rows;
    img._cols = mat.cols;
    img._data.resize(img._rows * img._cols);

    // 逐行逐列复制（按图像坐标系）
    if constexpr (N == 1) {
        for (std::size_t x = 0; x < img._rows; x++) {
            for (std::size_t y = 0; y < img._cols; y++) {
                img._data[x * img._cols + y][0] = mat.at<T>(x, y);
            }
        }
    } else if constexpr (N == 3) {
        for (std::size_t x = 0; x < img._rows; x++) {
            for (std::size_t y = 0; y < img._cols; y++) {
                auto& src = mat.at<cv::Vec<T, 3>>(x, y);
                auto& dst = img._data[x * img._cols + y];
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
            }
        }
    } else if constexpr (N == 4) {
        for (std::size_t x = 0; x < img._rows; x++) {
            for (std::size_t y = 0; y < img._cols; y++) {
                auto& src = mat.at<cv::Vec<T, 4>>(x, y);
                auto& dst = img._data[x * img._cols + y];
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = src[3];
            }
        }
    }

    return img;
}

// 将Image类转换为opencv的矩阵
template<std::size_t N, typename T>
cv::Mat Image<N, T>::toMat() const {
    int cv_type;
    if constexpr (N == 1) {
        cv_type = std::is_same_v<T, uint8_t> ? CV_8UC1 : CV_32FC1;
    } else if constexpr (N == 3) {
        cv_type = std::is_same_v<T, uint8_t> ? CV_8UC3 : CV_32FC3;
    } else if constexpr (N == 4) {
        cv_type = std::is_same_v<T, uint8_t> ? CV_8UC4 : CV_32FC4;
    } else {
        cv_type = CV_8UC1;
    }

    cv::Mat mat(_rows, _cols, cv_type);

    if constexpr (N == 1) {
        for (std::size_t x = 0; x < _rows; x++) {
            for (std::size_t y = 0; y < _cols; y++) {
                mat.at<T>(x, y) = at(x, y)[0];
            }
        }
    } else if constexpr (N == 3) {
        for (std::size_t x = 0; x < _rows; x++) {
            for (std::size_t y = 0; y < _cols; y++) {
                auto& pixel = at(x, y);
                mat.at<cv::Vec<T, 3>>(x, y) = {pixel[0], pixel[1], pixel[2]};
            }
        }
    } else if constexpr (N == 4) {
        for (std::size_t x = 0; x < _rows; x++) {
            for (std::size_t y = 0; y < _cols; y++) {
                auto& pixel = at(x, y);
                mat.at<cv::Vec<T, 4>>(x, y) = {pixel[0], pixel[1], pixel[2],
                                               pixel[3]};
            }
        }
    }

    return mat;
}

template<std::size_t N, typename T>
bool Image<N, T>::save(const fs::path& image_path) const {
    auto mat = toMat();
    if (mat.empty()) return false;
    return cv::imwrite(image_path.string(), mat);
}

// operator(): 可读写访问
// x = row (向下), y = col (向右)
template<std::size_t N, typename T>
typename Image<N, T>::pixel_type& Image<N, T>::operator()(std::size_t x,
                                                          std::size_t y) {
    if (x < _rows && y < _cols) {
        return _data[x * _cols + y];
    } else {
        throw std::out_of_range("Coordinates out of rang at function at().");
    }
}
template<std::size_t N, typename T>
typename Image<N, T>::pixel_type&
Image<N, T>::operator()(std::pair<std::size_t, std::size_t> coord) {
    return (*this)(coord.first, coord.second);
}

// at(): 只读访问
// x = row (向下), y = col (向右)
template<std::size_t N, typename T>
typename Image<N, T>::pixel_type Image<N, T>::at(std::size_t x,
                                                 std::size_t y) const {
    if (x < _rows && y < _cols) {
        return _data[x * _cols + y];
    } else {
        throw std::out_of_range(
            "Coordinates out of range at function operator().");
    }
}
template<std::size_t N, typename T>
typename Image<N, T>::pixel_type
Image<N, T>::at(std::pair<std::size_t, std::size_t> coord) const {
    return at(coord.first, coord.second);
}
template<std::size_t N, typename T>
typename Image<N, T>::pixel_type Image<N, T>::at_cut(int x, int y) const {
    if (x >= 0 && y >= 0 && x < _rows && y < _cols) {
        return _data[x * _cols + y];
    }
    if (x < 0) x = 0;
    if (x >= static_cast<int>(_rows)) x = _rows - 1;
    if (y < 0) y = 0;
    if (y >= static_cast<int>(_cols)) y = _cols - 1;
    return at(x, y);
}
template<std::size_t N, typename T>
typename Image<N, T>::pixel_type
Image<N, T>::at_cut(std::pair<int, int> coord) const {
    return at_cut(coord.first, coord.second);
}