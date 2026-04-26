# Effective Modern C++ 个人笔记

本项目是阅读 Scott Meyers 所著《Effective Modern C++》的个人学习笔记，包含各章节的知识整理以及配套的可运行示例代码。

## 项目结构

```
.
├── CMakeLists.txt
├── main.cpp              # 当前章节示例入口
└── DeducingTypes/
    ├── 1-Deducing Types.md   # Item 1-4 笔记
    └── DeducingTypes.h       # 类型推导示例模板函数
```

## 笔记目录

| 章节 | 主题 | 笔记 |
|------|------|------|
| Chapter 1 | 类型推导 | [DeducingTypes/1-Deducing Types.md](DeducingTypes/1-Deducing%20Types.md) |

## 构建与运行

依赖：CMake 3.15+，支持 C++17 的编译器（Clang / GCC）

```bash
mkdir build && cd build
cmake ..
cmake --build .
./main
```

## 参考资料

- 《Effective Modern C++》— Scott Meyers
- [cppreference.com](https://en.cppreference.com)
