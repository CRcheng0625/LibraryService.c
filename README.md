# My First C++ Project

这是一个用于学习现代 C++、数据结构和基础工程实践的命令行图书管理项目。

## 项目结构

```text
my first project/
├── app/                 # 程序入口和用户交互
├── include/library/     # 对外头文件：数据模型与业务接口
├── src/                 # 业务实现
├── tests/               # 自动化测试
├── docs/                # 学习任务与设计说明
├── CMakeLists.txt       # 构建规则
└── CMakePresets.json    # 统一的本地构建配置
```

依赖方向是 `app -> library_core`，测试也只依赖 `library_core`。这样用户界面、业务逻辑和测试不会混在一个文件里。

## 需要的工具

Windows 上推荐以下组合：

1. Visual Studio 2022 Community，安装时勾选“使用 C++ 的桌面开发”。它提供 MSVC 编译器和 Windows SDK。
2. CMake，用来生成和管理构建过程。
3. Ninja，用来执行快速构建；当前预设使用它。
4. Git，用来记录每一步学习和修改。
5. Visual Studio Code（可选），安装 C/C++ 与 CMake Tools 扩展。

如果更喜欢一个集成环境，可以直接用 Visual Studio 打开此文件夹；如果使用 VS Code，则需要单独确认编译器、CMake 和 Ninja 都在 `PATH` 中。

## 构建和运行

在项目根目录打开 PowerShell：

```powershell
cmake --preset default
cmake --build --preset default
.\build\library_cli.exe
```

运行测试：

```powershell
ctest --preset default
```

## 从哪里开始

1. 阅读 `include/library/book.hpp`，理解一本书包含哪些状态。
2. 阅读 `include/library/library_service.hpp`，只看它能做什么。
3. 阅读 `src/library_service.cpp`，再看每个功能怎么实现。
4. 阅读 `app/main.cpp`，观察界面如何调用业务功能。
5. 阅读并运行 `tests/library_service_test.cpp`。
6. 按 `docs/learning-roadmap.md` 完成第一个练习。

不要一开始追求图形界面、数据库或网络服务。先把一个小型命令行程序写完整、测完整，再逐层增加难度。
