#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

template<std::size_t N, typename T>
struct Pixel {
    T channels[N];

    Pixel() { std::fill_n(channels, N, T(0)); }

    explicit Pixel(T value) { std::fill_n(channels, N, value); }

    Pixel(std::initializer_list<T> list) {
        if (list.size() != N) {
            throw std::runtime_error("Pixel initializer list size mismatch");
        }
        std::copy(list.begin(), list.end(), channels);
    }

    // 访问运算符
    T& operator[](std::size_t i) { return channels[i]; }

    const T& operator[](std::size_t i) const { return channels[i]; }

    // 复合赋值运算符
    Pixel& operator+=(const Pixel& rhs) {
        for (std::size_t i = 0; i < N; ++i) channels[i] += rhs.channels[i];
        return *this;
    }

    Pixel& operator-=(const Pixel& rhs) {
        for (std::size_t i = 0; i < N; ++i) channels[i] -= rhs.channels[i];
        return *this;
    }

    Pixel& operator*=(const Pixel& rhs) {
        for (std::size_t i = 0; i < N; ++i) channels[i] *= rhs.channels[i];
        return *this;
    }

    Pixel& operator/=(const Pixel& rhs) {
        for (std::size_t i = 0; i < N; ++i) channels[i] /= rhs.channels[i];
        return *this;
    }

    // 标量运算
    Pixel& operator*=(T scalar) {
        for (std::size_t i = 0; i < N; ++i) channels[i] *= scalar;
        return *this;
    }

    Pixel& operator/=(T scalar) {
        for (std::size_t i = 0; i < N; ++i) channels[i] /= scalar;
        return *this;
    }

    // 二元运算符
    Pixel operator+(const Pixel& rhs) const {
        Pixel result = *this;
        result += rhs;
        return result;
    }

    Pixel operator-(const Pixel& rhs) const {
        Pixel result = *this;
        result -= rhs;
        return result;
    }

    Pixel operator*(const Pixel& rhs) const {
        Pixel result = *this;
        result *= rhs;
        return result;
    }

    Pixel operator/(const Pixel& rhs) const {
        Pixel result = *this;
        result /= rhs;
        return result;
    }

    Pixel operator*(T scalar) const {
        Pixel result = *this;
        result *= scalar;
        return result;
    }

    Pixel operator/(T scalar) const {
        Pixel result = *this;
        result /= scalar;
        return result;
    }

    // 比较运算符
    bool operator==(const Pixel& rhs) const {
        for (std::size_t i = 0; i < N; ++i) {
            if (channels[i] != rhs.channels[i]) return false;
        }
        return true;
    }

    bool operator!=(const Pixel& rhs) const { return !(*this == rhs); }

    // 输出
    friend std::ostream& operator<<(std::ostream& os, const Pixel& pixel) {
        os << "[";
        for (std::size_t i = 0; i < N; ++i) {
            os << pixel.channels[i];
            if (i != N - 1) os << ", ";
        }
        os << "]";
        return os;
    }
};

// 标量左乘
template<std::size_t N, typename T>
Pixel<N, T> operator*(T scalar, const Pixel<N, T>& pixel) {
    return pixel * scalar;
}

// PixelMatrix - 注意模板参数顺序: <typename T, std::size_t C>
template<typename T, std::size_t C>
class PixelMatrix {
public:
    using PixelType = Pixel<C, T>;

    // 构造函数
    PixelMatrix(): rows_(0), cols_(0) {}

    PixelMatrix(size_t rows, size_t cols):
        rows_(rows), cols_(cols), data_(rows * cols) {}

    PixelMatrix(size_t rows, size_t cols, const PixelType& init_value):
        rows_(rows), cols_(cols), data_(rows * cols, init_value) {}

    // 从初始化列表构造（用于小矩阵）
    PixelMatrix(std::initializer_list<std::initializer_list<PixelType>> list) {
        rows_ = list.size();
        if (rows_ == 0) {
            cols_ = 0;
            return;
        }
        cols_ = list.begin()->size();
        data_.reserve(rows_ * cols_);

        for (const auto& row: list) {
            if (row.size() != cols_) {
                throw std::runtime_error(
                    "Inconsistent row sizes in initializer list");
            }
            for (const auto& pixel: row) {
                data_.push_back(pixel);
            }
        }
    }

    // 基本属性
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    size_t size() const { return rows_ * cols_; }
    bool empty() const { return data_.empty(); }

    // 元素访问
    PixelType& at(size_t row, size_t col) {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("PixelMatrix index out of range");
        }
        return data_[row * cols_ + col];
    }

    const PixelType& at(size_t row, size_t col) const {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("PixelMatrix index out of range");
        }
        return data_[row * cols_ + col];
    }

    PixelType& operator()(size_t row, size_t col) {
        return data_[row * cols_ + col];
    }

    const PixelType& operator()(size_t row, size_t col) const {
        return data_[row * cols_ + col];
    }

    // 行访问
    std::vector<PixelType> row(size_t r) const {
        if (r >= rows_) throw std::out_of_range("Row index out of range");
        return std::vector<PixelType>(data_.begin() + r * cols_,
                                      data_.begin() + (r + 1) * cols_);
    }

    // 列访问
    std::vector<PixelType> col(size_t c) const {
        if (c >= cols_) throw std::out_of_range("Col index out of range");
        std::vector<PixelType> result;
        result.reserve(rows_);
        for (size_t i = 0; i < rows_; ++i) {
            result.push_back(data_[i * cols_ + c]);
        }
        return result;
    }

    // 矩阵运算
    PixelMatrix& operator+=(const PixelMatrix& rhs) {
        if (rows_ != rhs.rows_ || cols_ != rhs.cols_) {
            throw std::runtime_error("Matrix size mismatch for addition");
        }
        for (size_t i = 0; i < data_.size(); ++i) {
            data_[i] += rhs.data_[i];
        }
        return *this;
    }

    PixelMatrix& operator-=(const PixelMatrix& rhs) {
        if (rows_ != rhs.rows_ || cols_ != rhs.cols_) {
            throw std::runtime_error("Matrix size mismatch for subtraction");
        }
        for (size_t i = 0; i < data_.size(); ++i) {
            data_[i] -= rhs.data_[i];
        }
        return *this;
    }

    PixelMatrix& operator*=(T scalar) {
        for (size_t i = 0; i < data_.size(); ++i) {
            data_[i] *= scalar;
        }
        return *this;
    }

    PixelMatrix& operator/=(T scalar) {
        for (size_t i = 0; i < data_.size(); ++i) {
            data_[i] /= scalar;
        }
        return *this;
    }

    PixelMatrix operator+(const PixelMatrix& rhs) const {
        PixelMatrix result = *this;
        result += rhs;
        return result;
    }

    PixelMatrix operator-(const PixelMatrix& rhs) const {
        PixelMatrix result = *this;
        result -= rhs;
        return result;
    }

    PixelMatrix operator*(T scalar) const {
        PixelMatrix result = *this;
        result *= scalar;
        return result;
    }

    // 矩阵乘法（像素级乘法，不是矩阵乘法）
    PixelMatrix elementwiseMul(const PixelMatrix& rhs) const {
        if (rows_ != rhs.rows_ || cols_ != rhs.cols_) {
            throw std::runtime_error(
                "Matrix size mismatch for elementwise multiplication");
        }
        PixelMatrix result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            result.data_[i] = data_[i] * rhs.data_[i];
        }
        return result;
    }

    // 获取子矩阵
    PixelMatrix submatrix(size_t row_start, size_t col_start, size_t row_end,
                          size_t col_end) const {
        if (row_start >= rows_ || col_start >= cols_ || row_end > rows_ ||
            col_end > cols_ || row_start >= row_end || col_start >= col_end) {
            throw std::out_of_range("Invalid submatrix range");
        }

        size_t sub_rows = row_end - row_start;
        size_t sub_cols = col_end - col_start;
        PixelMatrix result(sub_rows, sub_cols);

        for (size_t i = 0; i < sub_rows; ++i) {
            for (size_t j = 0; j < sub_cols; ++j) {
                result(i, j) = (*this)(row_start + i, col_start + j);
            }
        }
        return result;
    }

    // 重置大小
    void resize(size_t new_rows, size_t new_cols) {
        rows_ = new_rows;
        cols_ = new_cols;
        data_.resize(rows_ * cols_);
    }

    void resize(size_t new_rows, size_t new_cols, const PixelType& init_value) {
        rows_ = new_rows;
        cols_ = new_cols;
        data_.assign(rows_ * cols_, init_value);
    }

    // 填充
    void fill(const PixelType& value) {
        std::fill(data_.begin(), data_.end(), value);
    }

    // 统计
    PixelType min() const {
        if (data_.empty()) return PixelType();
        PixelType result = data_[0];
        for (const auto& p: data_) {
            for (size_t c = 0; c < C; ++c) {
                if (p[c] < result[c]) result[c] = p[c];
            }
        }
        return result;
    }

    PixelType max() const {
        if (data_.empty()) return PixelType();
        PixelType result = data_[0];
        for (const auto& p: data_) {
            for (size_t c = 0; c < C; ++c) {
                if (p[c] > result[c]) result[c] = p[c];
            }
        }
        return result;
    }

    PixelType mean() const {
        if (data_.empty()) return PixelType();
        PixelType sum;
        for (const auto& p: data_) {
            sum += p;
        }
        return sum / static_cast<T>(data_.size());
    }

    // 数据访问
    std::vector<PixelType>& data() { return data_; }
    const std::vector<PixelType>& data() const { return data_; }
    T* raw_data() { return reinterpret_cast<T*>(data_.data()); }
    const T* raw_data() const {
        return reinterpret_cast<const T*>(data_.data());
    }

    // 输出
    friend std::ostream& operator<<(std::ostream& os, const PixelMatrix& mat) {
        for (size_t i = 0; i < mat.rows_; ++i) {
            os << "[";
            for (size_t j = 0; j < mat.cols_; ++j) {
                os << mat(i, j);
                if (j != mat.cols_ - 1) os << ", ";
            }
            os << "]";
            if (i != mat.rows_ - 1) os << "\n";
        }
        return os;
    }

private:
    size_t rows_, cols_;
    std::vector<PixelType> data_;
};

// 卷积函数 - 修正模板参数顺序
template<typename T, std::size_t C>
PixelMatrix<T, C> convolve(const PixelMatrix<T, C>& input,
                           const PixelMatrix<T, 1>& kernel) {
    size_t in_rows = input.rows();
    size_t in_cols = input.cols();
    size_t k_rows = kernel.rows();
    size_t k_cols = kernel.cols();

    if (k_rows % 2 == 0 || k_cols % 2 == 0) {
        throw std::runtime_error("Kernel dimensions must be odd");
    }

    size_t out_rows = in_rows - k_rows + 1;
    size_t out_cols = in_cols - k_cols + 1;
    PixelMatrix<T, C> result(out_rows, out_cols);

    for (size_t i = 0; i < out_rows; ++i) {
        for (size_t j = 0; j < out_cols; ++j) {
            typename PixelMatrix<T, C>::PixelType sum;
            for (size_t ki = 0; ki < k_rows; ++ki) {
                for (size_t kj = 0; kj < k_cols; ++kj) {
                    size_t in_i = i + ki;
                    size_t in_j = j + kj;
                    T kernel_weight = kernel(in_i, in_j)[0];
                    sum = sum + input(in_i, in_j) * kernel_weight;
                }
            }
            result(i, j) = sum;
        }
    }

    return result;
}