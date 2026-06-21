# 数字图像处理作业

基于 C++ 和 OpenCV 的数字图像处理作业项目，实现图像插值、直方图均衡化、中值滤波和拉普拉斯锐化等功能。

## 依赖

- OpenCV 4.x
- CMake 3.10+
- C++17 兼容编译器（GCC / Clang / MSVC）

## 编译与构建

```bash
# 配置 + 构建 + 编译（一步完成）
make release-build

# 或者分步执行
make release          # 配置 Release 模式
make build            # 编译

# Debug 模式
make debug-build
```

编译产物位于 `out/bin/digitalImageProcessing`，项目根目录会自动创建软链接，可直接运行 `./digitalImageProcessing`。

## 命令行用法

```bash
./digitalImageProcessing <模式> <输入文件夹路径>
```

| 模式 | 对应作业 | 说明 |
|------|----------|------|
| `1` | 作业 1 | 图像插值（最近邻 / 双线性 / 双三次，2 种缩放倍数） |
| `2-1` | 作业 2-1 | 迭代直方图均衡化 |
| `2-2` | 作业 2-2 | 迭代中值滤波 |
| `2-3` | 作业 2-3 | 拉普拉斯锐化 |

### 示例

```bash
# 作业 1：插值
./digitalImageProcessing 1 data/01/interpolation/input

# 作业 2-1：直方图均衡化 → 只处理 bridge.jpg
./digitalImageProcessing 2-1 data/02/histogramEqualization/input

# 作业 2-2：中值滤波 → 只处理 circuit.jpg
./digitalImageProcessing 2-2 data/02/medianFilter/input

# 作业 2-3：拉普拉斯锐化 → 只处理 moon.jpg
./digitalImageProcessing 2-3 data/02/laplacianSharpen/input
```

## 目录结构

```
digitalImageProcessing/
├── Source/digitalImageProcessing/
│   ├── include/
│   │   ├── pixel.h           # Pixel 和 PixelMatrix 模板
│   │   ├── image.h           # Image 模板类
│   │   ├── interpolators.h   # 插值算法（最近邻 / 双线性 / 双三次）
│   │   ├── histogram.h       # 直方图均衡化（含迭代版本）
│   │   ├── median_filter.h   # 中值滤波（含迭代版本）
│   │   ├── laplacian.h       # 拉普拉斯锐化（4邻域 / 8邻域）
│   │   └── tests.h           # 测试函数（统一入口）
│   └── src/
│       └── main.cpp          # 主函数（命令行调度）
├── data/
│   ├── 01/interpolation/     # 作业 1：插值
│   │   ├── input/            #   输入图片
│   │   └── output/           #   输出结果
│   └── 02/                   # 作业 2
│       ├── histogramEqualization/
│       │   ├── input/        #   2-1 输入：bridge.jpg
│       │   └── output/       #   2-1 输出
│       ├── medianFilter/
│       │   ├── input/        #   2-2 输入：circuit.jpg
│       │   └── output/       #   2-2 输出
│       └── laplacianSharpen/
│           ├── input/        #   2-3 输入：moon.jpg
│           └── output/       #   2-3 输出
├── out/bin/                  # 编译产物
├── Makefile
├── CMakeLists.txt
└── README.md
```

## 输出命名规范

输出文件自动生成在同级 `output/` 目录下，命名规则如下。

### 作业 1：图像插值

```
{原文件名}_processed_{插值方法}_{缩放倍数}.{扩展名}
```

| 缩写 | 插值方法 | 缩放倍数 |
|------|----------|----------|
| `NNI` | 最近邻插值 (Nearest Neighbor) | `2.000000` / `0.500000` |
| `BLI` | 双线性插值 (Bi-linear) | `2.000000` / `0.500000` |
| `BCI` | 双三次插值 (Bi-cubic) | `2.000000` / `0.500000` |

**示例：**
- `lenna_processed_NNI_2.000000.jpg`
- `lenna_processed_BLI_0.500000.jpg`

### 作业 2-1：迭代直方图均衡化

```
{原文件名}_processed_HE_n{次数}.{扩展名}
{原文件名}_processed_HE_inf_{收敛迭代数}iter.{扩展名}
```

迭代次数：`1`, `3`, `5`, `10`，外加收敛版本（自动检测图像不再变化时停止）。

**示例：**
- `bridge_processed_HE_n1.jpg`
- `bridge_processed_HE_inf_2iter.jpg`

### 作业 2-2：迭代中值滤波

```
{原文件名}_processed_MF_n{次数}.{扩展名}
{原文件名}_processed_MF_inf_{收敛迭代数}iter.{扩展名}
```

迭代次数：`1`, `3`, `5`, `10`，外加收敛版本。滤波核大小固定为 3×3。

**示例：**
- `circuit_processed_MF_n3.jpg`
- `circuit_processed_MF_inf_50iter.jpg`

### 作业 2-3：拉普拉斯锐化

```
{原文件名}_processed_LS_{核类型}_s{强度}.{扩展名}
```

| 核类型 | 说明 |
|--------|------|
| `L4` | 4 邻域拉普拉斯核 |
| `L8` | 8 邻域拉普拉斯核 |

锐化强度：`0.5`, `1.0`, `1.5`, `2.0`。

**示例：**
- `moon_processed_LS_L4_s1.000000.jpg`
- `moon_processed_LS_L8_s2.000000.jpg`

## 作业 2 图片分配

| 作业 | 模式参数 | 输入图片 |
|------|----------|----------|
| 2-1 直方图均衡化 | `2-1` | `bridge.jpg` |
| 2-2 中值滤波 | `2-2` | `circuit.jpg` |
| 2-3 拉普拉斯锐化 | `2-3` | `moon.jpg` |
