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
- 使用 GoogleTest 编写应用层文件操作测试
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
│   ├── book_repository.hpp         # 存储仓库抽象接口和文件实现
│   ├── book_validation.hpp         # 图书数据校验接口
│   └── library_service.hpp         # 图书业务接口
├── src/
│   ├── book_storage.cpp            # 文件读写实现
│   ├── book_validation.cpp         # 图书数据校验实现
│   ├── library_service.cpp         # 图书业务实现
│   └── mysql_book_repository.cpp   # MySQL 存储实现（可选构建）
├── tests/
│   ├── library_service_gtest.cpp   # LibraryService 测试
│   ├── book_storage_gtest.cpp      # 文件持久化测试
│   ├── book_validation_gtest.cpp   # 图书校验测试
│   ├── library_io_gtest.cpp        # 应用层文件操作测试
│   ├── check_cli_output.cmake      # CLI 输出和退出码检查脚本
│   └── data/                       # CLI 测试使用的固定数据文件
│       ├── books.txt
│       ├── custom_books.txt
│       └── invalid/books.txt
├── docs/
│   └── learning-roadmap.md         # 学习路线和练习记录
├── database/
│   └── schema.sql                   # MySQL 数据库和 books 表结构
├── CMakeLists.txt                  # CMake 构建规则
└── CMakePresets.json               # 默认构建预设
```

依赖关系如下：

```text
library_service.cpp ─┐
book_storage.cpp ────┼──> library_core ───> library_cli
book_validation.cpp ─┘                          ▲
                                                │
                         cli_option.cpp ────────┤
                         library_io.cpp ────────┘
library_service_gtest ─┐
book_storage_gtest ────┤
book_validation_gtest ─┴──> GoogleTest + CTest
```

业务逻辑位于 `library_core`，命令行程序和测试程序通过 CMake 链接这个库。

`BookRepository` 表示“图书保存位置”的统一接口，当前的 `FileBookRepository` 将数据保存到文本文件。后续接入 MySQL 时，可以新增 `MySqlBookRepository`，而不需要修改借书、搜索和菜单逻辑。

MySQL 表结构保存在 `database/schema.sql`。在 MySQL Workbench 中打开该文件并执行，可以自动创建 `library_app` 数据库和 `books` 表；这比手动逐行输入更容易重复，也会把项目需要的数据库结构提交到 Git。

如果已经安装了 Connector/C++，可以打开可选的连接检查程序：

```powershell
cmake --preset mysql-release
cmake --build --preset mysql-release --target mysql_connection_check
```

连接检查程序从环境变量读取 `MYSQL_HOST`、`MYSQL_USER`、`MYSQL_PASSWORD` 和 `MYSQL_SCHEMA`，不会把密码写进源码。

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

只运行命令行测试：

```powershell
ctest --test-dir build --label-regex "^cli$" --output-on-failure
```

测试标签只用于筛选，不会改变测试行为。

安装可交付文件到单独目录：

```powershell
cmake --install build --prefix install
```

安装后，运行程序位于 `install/bin/library_cli.exe`，示例数据位于
`install/share/first_cpp_project/books.txt`。GoogleTest 只用于开发和测试，不会被安装。

打包成 ZIP 文件：

```powershell
cpack --config build/CPackConfig.cmake
```

打包文件会生成在 `build/first_cpp_project-0.1.0.zip`。

GitHub Actions 会在 Linux 和 Windows 两个平台分别构建、测试并上传 ZIP artifact。

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

非交互列出图书：

```powershell
.\build\library_cli.exe --list tests\data\custom_books.txt
```

`--list` 会读取指定文件并打印全部图书，然后直接退出，不进入交互菜单；省略文件路径时使用默认的 `books.txt`。

统计图书数量：

```powershell
.\build\library_cli.exe --count tests\data\custom_books.txt
```

`--count` 会读取文件并输出图书总数，然后直接退出。

查看图书统计：

```powershell
.\build\library_cli.exe --stats tests\data\books.txt
```

`--stats` 会输出总数、可借数量、已借数量和借阅率，然后直接退出。

只列出当前可借的图书：

```powershell
.\build\library_cli.exe --available tests\data\custom_books.txt
```

`--available` 会读取文件并打印未借出的图书，然后直接退出；已借出的图书不会显示。

按标题关键词搜索（忽略大小写）：

```powershell
.\build\library_cli.exe --search-title "C++" tests\data\books.txt
```

按作者关键词搜索（忽略大小写）：

```powershell
.\build\library_cli.exe --search-author "Martin" tests\data\books.txt
```

这两个命令都要求提供关键词，文件路径是可选的；省略文件路径时使用默认的 `books.txt`。程序会先输出匹配数量，再输出每本匹配的图书。

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
8. `tests/library_service_gtest.cpp`、`tests/book_storage_gtest.cpp`、`tests/book_validation_gtest.cpp` 与 `tests/library_io_gtest.cpp`：理解 GoogleTest 如何验证不同层的行为。
9. `docs/learning-roadmap.md`：按练习路线继续扩展。

## 学习原则

先让程序运行，再逐步重构；每个功能都经过测试；每次 Git 提交只包含一个清晰的小目标。遇到不会的代码时，先说明输入、输出和执行流程，再研究实现细节。

## 当前状态

第一阶段学习项目已经完成，主线包含核心图书功能、组合筛选、分页、CMake 模块化、GoogleTest、CTest、Git 工作流、静态检查和文件持久化。下一阶段重点是减少提示，独立完成需求拆解、实现、测试和提交。
