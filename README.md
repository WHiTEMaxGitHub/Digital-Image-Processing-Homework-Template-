# 数字图像处理作业 — 作业3：金字塔 / 小波变换 / 小波边缘检测

基于 C++ 和 OpenCV 的数字图像处理模板项目。本分支为 **作业3** 的代码模板。

> 💡 **代码模板分支**（含 TODO 骨架、Doxygen 注释、算法步骤提示）：
> - `template/homework1` — 作业1：图像插值
> - `template/homework2` — 作业2：直方图均衡化 / 中值滤波 / 拉普拉斯锐化
> - `template/homework3` — 作业3：金字塔 / 小波变换 / 小波边缘检测
> - `template/homework4` — 作业4：形态学处理
>
> 切换到对应分支后，只需实现标记为 TODO 的核心算法函数即可编译运行。

## 依赖

- OpenCV 4.x
- CMake 3.10+
- C++17 兼容编译器（GCC / Clang / MSVC）

## 编译与构建

项目支持 Release 和 Debug 两种模式，`make build` 使用当前已配置的模式进行编译。

```bash
# === 分步执行 ===
make release          # 配置 Release 模式
make build            # 编译
# 或者
make debug            # 配置 Debug 模式
make build            # 编译

# === 快捷执行（配置 + 编译） ===
make release-build    # Release 模式一步到位
make debug-build      # Debug 模式一步到位
```

编译产物位于 `out/bin/digitalImageProcessing`，项目根目录会自动创建软链接，可直接运行 `./digitalImageProcessing`。

## 命令行用法

```bash
./digitalImageProcessing <模式> <输入文件夹路径>
```

| 模式 | 对应作业 | 说明 |
|------|----------|------|
| `1`   | 作业 1   | 图像插值（最近邻 / 双线性 / 双三次，2 种缩放倍数） |
| `2-1` | 作业 2-1 | 迭代直方图均衡化 |
| `2-2` | 作业 2-2 | 迭代中值滤波 |
| `2-3` | 作业 2-3 | 拉普拉斯锐化 |
| `3-1` | 作业 3-1 | 近似金字塔与预测残差金字塔（教材例7.1） |
| `3-2` | 作业 3-2 | 二维快速小波变换 FWT（教材例7.12） |
| `3-3` | 作业 3-3 | 基于小波的边缘检测（教材例7.13） |
| `4-1` | 作业 4-1 | 二值形态学（长字符提取、空洞填充、边界清除） |
| `4-2` | 作业 4-2 | 灰度形态学（顶帽变换、粒度测定、分水岭预处理） |

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

# 作业 3-1：金字塔 → 处理 demo-1.jpg
./digitalImageProcessing 3-1 data/03/pyramid/input

# 作业 3-2：小波变换 → 处理 demo-2.tif
./digitalImageProcessing 3-2 data/03/wavelet/input

# 作业 3-3：小波边缘检测 → 处理 demo-2.tif
./digitalImageProcessing 3-3 data/03/wavelet/input

# 作业 4-1：二值形态学 → 处理 Fig0929(a)(text_image).tif
./digitalImageProcessing 4-1 data/04/binary/input

# 作业 4-2：灰度形态学 → 处理 3 张灰度图像
./digitalImageProcessing 4-2 data/04/gray/input
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
│   │   ├── pyramid.h         # 金字塔（近似 / 预测残差）
│   │   ├── wavelet.h         # 小波变换 + 小波边缘检测
│   │   ├── morphology.h      # 形态学（二值 + 灰度）
│   │   └── tests.h           # 测试函数（统一入口）
│   └── src/
│       └── main.cpp          # 主函数（命令行调度）
├── data/
│   ├── 01/interpolation/     # 作业 1：插值
│   │   ├── input/            #   输入图片
│   │   └── output/           #   输出结果
│   ├── 02/                   # 作业 2
│   │   ├── histogramEqualization/
│   │   │   ├── input/        #   2-1 输入：bridge.jpg
│   │   │   └── output/       #   2-1 输出
│   │   ├── medianFilter/
│   │   │   ├── input/        #   2-2 输入：circuit.jpg
│   │   │   └── output/       #   2-2 输出
│   │   └── laplacianSharpen/
│   │       ├── input/        #   2-3 输入：moon.jpg
│   │       └── output/       #   2-3 输出
│   ├── 03/                   # 作业 3
│   │   ├── pyramid/
│   │   │   ├── input/        #   3-1 输入：demo-1.jpg
│   │   │   └── output/       #   3-1 输出
│   │   └── wavelet/
│   │       ├── input/        #   3-2/3-3 输入：demo-2.tif
│   │       └── output/       #   3-2 输出（小波变换拼接图）
│   └── 04/                   # 作业 4
│       ├── binary/
│       │   ├── input/        #   4-1 输入：Fig0929(a)(text_image).tif
│       │   └── output/       #   4-1 输出
│       └── gray/
│           ├── input/        #   4-2 输入：Fig0940(a)/Fig0941(a)/Fig0943(a)
│           └── output/       #   4-2 输出
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

### 作业 3-1：金字塔

```
{原文件名}_approx_lv{级数}.{扩展名}
{原文件名}_residual_lv{级数}.{扩展名}
```

金字塔共 5 级（lv0 ~ lv4），lv0 为原图分辨率，每级缩小 2×。残差图像已加 128 偏移以便可视化。

**示例：**
- `demo-1_approx_lv0.jpg`（512×512，原图）
- `demo-1_approx_lv4.jpg`（32×32）
- `demo-1_residual_lv1.jpg`（256×256 预测残差）

### 作业 3-2：二维快速小波变换

```
{原文件名}_fwt_lv{级数}.{扩展名}
```

输出为子带拼接图（2×2 排列：左上 LL、右上 LH、左下 HL、右下 HH），多级分解时左上角递归分解。默认使用 db4 小波。

**示例：**
- `demo-2_fwt_lv1.tif`（1 级分解拼接图）
- `demo-2_fwt_lv3.tif`（3 级分解拼接图）

### 作业 3-3：小波边缘检测

```
{原文件名}_edge_th{阈值}.{扩展名}
```

基于小波分解高频子带（LH/HL/HH）的模值检测边缘，使用阈值因子滤除弱响应。
阈值因子：`1.0`, `1.5`, `2.0`。

**示例：**
- `demo-2_edge_th1.0.tif`
- `demo-2_edge_th2.0.tif`

### 作业 4-1：二值形态学

```
{原文件名}_long_char.{扩展名}    # 长字符提取（图9.29）
{原文件名}_hole_fill.{扩展名}    # 空洞填充（图9.31）
{原文件名}_border_clear.{扩展名} # 边界清除（图9.32）
```

先进行阈值二值化（th=128），再分别执行对应的形态学操作。

**示例：**
- `Fig0929(a)(text_image)_long_char.tif`
- `Fig0929(a)(text_image)_hole_fill.tif`
- `Fig0929(a)(text_image)_border_clear.tif`

### 作业 4-2：灰度形态学

```
{原文件名}_tophat.{扩展名}         # 顶帽变换（图9.40）
{原文件名}_tophat_binary.{扩展名}  # 顶帽 + Otsu 阈值分割
{原文件名}_granulometry.{扩展名}   # 粒度测定（图9.41）
{原文件名}_watershed_grad.{扩展名} # 分水岭梯度（图9.43）
{原文件名}_watershed_markers.{扩展名} # 分水岭标记
```

程序根据输入图片文件名自动匹配对应的操作。

**示例：**
- `Fig0940_tophat.tif`
- `Fig0941_granulometry.tif`
- `Fig0943_watershed_grad.tif`

## 图片分配总表

| 作业 | 模式参数 | 输入图片 | 尺寸 |
|------|----------|----------|------|
| 1 插值 | `1` | `building.jpg`, `cameraman.jpg`, `lenna.jpg` | — |
| 2-1 直方图均衡化 | `2-1` | `bridge.jpg` | — |
| 2-2 中值滤波 | `2-2` | `circuit.jpg` | — |
| 2-3 拉普拉斯锐化 | `2-3` | `moon.jpg` | — |
| 3-1 金字塔 | `3-1` | `demo-1.jpg` | 512×512 |
| 3-2 小波变换 | `3-2` | `demo-2.tif` | 128×128 |
| 3-3 小波边缘检测 | `3-3` | `demo-2.tif` | 128×128 |
| 4-1 二值形态学 | `4-1` | `Fig0929(a)(text_image).tif` | — |
| 4-2 灰度形态学 | `4-2` | `Fig0940(a)`, `Fig0941(a)`, `Fig0943(a)` | — |
