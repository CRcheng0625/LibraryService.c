# My First C++ Project--图书管理系统

这是一个用于学习现代 C++ 和基础工程实践的命令行图书管理项目。项目从一个简单的 `vector<Book>` 开始，逐步加入业务类、STL 算法、CMake、单元测试、调试、Git 工作流和文件持久化。

## 当前功能

- 添加、删除图书
- 借书和还书
- 按 ID 查找图书
- 按标题或作者关键词搜索，忽略大小写
- 按出版年份查询和排序
- 按作者与出版年份组合查询
- 使用 `BookFilter` 组合标题、作者、年份和借阅状态筛选
- 使用 `BookPage` 分页读取图书列表
- 使用 `unordered_map` 索引加速按 ID 查找
- 使用 `LoanResult` 和 `SaveStatus` 返回具体的成功或失败原因
- 启动时从 `books.txt` 读取数据，退出时保存数据
- 使用 CTest 运行自动化测试
- 使用 GoogleTest 编写服务层和持久化测试
- 使用共享的 `book_validation` 模块统一校验图书数据
- 使用 `.clang-format` 统一 C++ 代码风格
- 使用 clang-tidy 和 GitHub Actions 做持续检查

## 项目结构

```text
my first project/
├── app/
│   ├── main.cpp                    # 菜单、输入处理和程序入口
│   ├── cli_option.cpp              # 命令行参数实现
│   ├── cli_options.hpp             # 命令行参数声明
│   ├── library_io.cpp              # 文件保存、加载和检查实现
│   └── library_io.hpp              # 文件操作声明
├── include/library/
│   ├── book.hpp                    # Book 数据类型
│   ├── book_storage.hpp            # 文件持久化接口
│   ├── book_validation.hpp         # 图书数据校验接口
│   └── library_service.hpp         # 图书业务接口
├── src/
│   ├── book_storage.cpp            # 文件读写实现
│   ├── book_validation.cpp         # 图书数据校验实现
│   └── library_service.cpp         # 图书业务实现
├── tests/
│   ├── library_service_gtest.cpp   # LibraryService 测试
│   ├── book_storage_gtest.cpp      # 文件持久化测试
│   ├── book_validation_gtest.cpp   # 图书校验测试
│   └── data/                       # CLI 测试使用的固定数据文件
│       ├── books.txt
│       ├── custom_books.txt
│       └── invalid/books.txt
├── docs/
│   └── learning-roadmap.md         # 学习路线和练习记录
├── CMakeLists.txt                  # CMake 构建规则
└── CMakePresets.json               # 默认构建预设
```

依赖关系如下：

```text
library_service.cpp ─┐
book_storage.cpp ────┼──> library_core ───> library_cli
book_validation.cpp ─┘                         ▲
                                               │
                         cli_option.cpp ───────┤
                         library_io.cpp ────────┘
library_service_gtest ─┐
book_storage_gtest ────┤
book_validation_gtest ─┴──> GoogleTest + CTest
```

业务逻辑位于 `library_core`，命令行程序和测试程序通过 CMake 链接这个库。

## 工具

Windows 开发环境需要：

- Visual Studio，包含 MSVC、Windows SDK 和 C++ CMake 工具
- CMake
- Ninja
- Git

Visual Studio 已经可以提供其中的大部分组件。也可以使用其他编辑器，但必须能找到 C++ 编译器、CMake 和 Ninja。第一次配置时，CMake 会从 GitHub 下载固定版本的 GoogleTest。

## 构建、运行和测试

在项目根目录打开开发者 PowerShell 或 Visual Studio 终端：

```powershell
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure
```

也可以直接使用 CTest 的构建目录：

```powershell
ctest --test-dir build --output-on-failure
```

安装可交付文件到单独目录：

```powershell
cmake --install build --prefix install
```

安装后，运行程序位于 `install/bin/library_cli.exe`，示例数据位于
`install/share/first_cpp_project/books.txt`。GoogleTest 只用于开发和测试，不会被安装。

项目根目录的 `.clang-format` 定义了统一格式。Visual Studio 安装 clang-format 后，可以对当前文件执行格式化；格式化属于代码风格调整，不改变业务逻辑。

运行命令行程序：

```powershell
.\build\library_cli.exe
```

查看帮助和版本：

```powershell
.\build\library_cli.exe --help
.\build\library_cli.exe --version
```

检查默认数据文件：

```powershell
.\build\library_cli.exe --check
```

检查指定数据文件：

```powershell
.\build\library_cli.exe --check tests\data\custom_books.txt
```

数据文件格式：

```text
id "title" "author" publication_year borrowed
```

例如：

```text
1 "C++ Primer" "Stanley Lippman" 2012 0
2 "Clean Code" "Robert C. Martin" 2008 1
```

其中 `0` 表示未借出，`1` 表示已借出。`tests/data/` 中的文件只用于自动测试；程序运行时生成的 `books.txt` 通常位于 `build/`，不会提交到 Git。

## 推荐阅读顺序

1. `include/library/book.hpp`：理解数据结构。
2. `include/library/library_service.hpp`：理解业务类提供的接口。
3. `src/library_service.cpp`：理解查找、筛选、排序和状态修改。
4. `app/main.cpp`：理解菜单、输入处理和业务调用。
5. `app/cli_options.hpp` 与 `app/cli_option.cpp`：理解命令行参数解析。
6. `app/library_io.hpp` 与 `app/library_io.cpp`：理解应用层如何调用文件持久化接口。
7. `include/library/book_storage.hpp` 与 `src/book_storage.cpp`：理解文件保存和读取。
8. `tests/library_service_gtest.cpp`、`tests/book_storage_gtest.cpp` 与 `tests/book_validation_gtest.cpp`：理解 GoogleTest 如何验证行为。
9. `docs/learning-roadmap.md`：按练习路线继续扩展。

## 学习原则

先让程序运行，再逐步重构；每个功能都经过测试；每次 Git 提交只包含一个清晰的小目标。遇到不会的代码时，先说明输入、输出和执行流程，再研究实现细节。

## 当前状态

第一阶段学习项目已经完成，主线包含核心图书功能、组合筛选、分页、CMake 模块化、GoogleTest、CTest、Git 工作流、静态检查和文件持久化。下一阶段重点是减少提示，独立完成需求拆解、实现、测试和提交。
