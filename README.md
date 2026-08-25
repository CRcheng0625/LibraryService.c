# My First C++ Project

这是一个用于学习现代 C++ 和基础工程实践的命令行图书管理项目。项目从一个简单的 `vector<Book>` 开始，逐步加入业务类、STL 算法、CMake、单元测试、调试、Git 工作流和文件持久化。

## 当前功能

- 添加、删除图书
- 借书和还书
- 按 ID 查找图书
- 按标题或作者关键词搜索，忽略大小写
- 按出版年份查询和排序
- 按作者与出版年份组合查询
- 启动时从 `books.txt` 读取数据，退出时保存数据
- 使用 CTest 运行自动化测试
- 使用 `.clang-format` 统一 C++ 代码风格

## 项目结构

```text
my first project/
├── app/
│   └── main.cpp                    # 命令行界面和程序入口
├── include/library/
│   ├── book.hpp                    # Book 数据类型
│   ├── book_storage.hpp            # 文件持久化接口
│   └── library_service.hpp         # 图书业务接口
├── src/
│   ├── book_storage.cpp            # 文件读写实现
│   └── library_service.cpp         # 图书业务实现
├── tests/
│   └── library_service_test.cpp    # 单元测试
├── docs/
│   └── learning-roadmap.md         # 学习路线和练习记录
├── CMakeLists.txt                  # CMake 构建规则
└── CMakePresets.json               # 默认构建预设
```

依赖关系如下：

```text
library_service.cpp ─┐
book_storage.cpp ────┼──> library_core
                     ├──> library_cli
library_service_test ─┘     library_tests
```

业务逻辑位于 `library_core`，命令行程序和测试程序通过 CMake 链接这个库。

## 工具

Windows 开发环境需要：

- Visual Studio，包含 MSVC、Windows SDK 和 C++ CMake 工具
- CMake
- Ninja
- Git

Visual Studio 已经可以提供其中的大部分组件。也可以使用其他编辑器，但必须能找到 C++ 编译器、CMake 和 Ninja。

## 构建、运行和测试

在项目根目录打开开发者 PowerShell 或 Visual Studio 终端：

```powershell
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure
```

项目根目录的 `.clang-format` 定义了统一格式。Visual Studio 安装 clang-format 后，可以对当前文件执行格式化；格式化属于代码风格调整，不改变业务逻辑。

运行命令行程序：

```powershell
.\build\library_cli.exe
```

程序运行时生成的 `books.txt` 位于当前工作目录，通常是 `build/books.txt`。它是运行时数据，不应提交到 Git。

## 推荐阅读顺序

1. `include/library/book.hpp`：理解数据结构。
2. `include/library/library_service.hpp`：理解业务类提供的接口。
3. `src/library_service.cpp`：理解查找、筛选、排序和状态修改。
4. `app/main.cpp`：理解菜单、输入处理和业务调用。
5. `include/library/book_storage.hpp` 与 `src/book_storage.cpp`：理解文件保存和读取。
6. `tests/library_service_test.cpp`：理解测试如何验证行为。
7. `docs/learning-roadmap.md`：按练习路线继续扩展。

## 学习原则

先让程序运行，再逐步重构；每个功能都经过测试；每次 Git 提交只包含一个清晰的小目标。遇到不会的代码时，先说明输入、输出和执行流程，再研究实现细节。

## 当前状态

第一阶段学习项目已经完成，主线包含核心图书功能、CMake 模块化、CTest 测试、Git 工作流和文件持久化。下一阶段重点是减少提示，独立完成需求拆解、实现、测试和提交。
