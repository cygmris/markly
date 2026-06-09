# Markly — Foundation (app-foundation spec)

Markly 是 VNote 的 Qt 6.8 重写版。本文档记录地基层的构建/运行/配置信息。

## 构建

```bash
cd /home/eason/workflow/cygmris/cikatail/markly
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug   # 或 Release
cmake --build build
```

依赖：Qt 6.8（Core/Gui/Widgets/Network/Test）、CMake ≥ 3.20、Ninja、gcc/clang。

产物：`build/src/markly`（可执行）、`build/src/libmarkly_core.a`（核心静态库）。

## 运行

```bash
./build/src/markly                 # 启动主窗口（占位）
./build/src/markly --version       # 打印版本
./build/src/markly --help          # 帮助
./build/src/markly --verbose       # 详细日志
./build/src/markly --log-stderr    # 日志输出到 stderr（默认输出到日志文件）
./build/src/markly --watch-themes  # 监听主题文件夹热重载（spec #2 起生效）
./build/src/markly <file.md> ...   # 打开文件（已运行实例会复用并转发）
```

无显示环境下用 `QT_QPA_PLATFORM=offscreen` 运行。

## 测试

```bash
cd build && ctest --output-on-failure
```

- `test_configmgr`：配置合并/路径解析/损坏 JSON 回退/update 落盘/session 往返。
- `test_commandlineoptions`：参数解析与 version/help/error。
- `test_singleinstanceguard`：单实例 + IPC（show/openFiles）。

## 配置与日志路径

- 用户配置目录：`~/.config/Markly/Markly/`（org 与 app 名均为 `Markly`）。
  - `markly.json`：主配置（metadata/version/core/editor/widget）。
  - `session.json`：会话（窗口几何/状态、默认笔记本根目录）——窗口正常关闭时写入。
  - `markly.log`：日志文件（`--log-stderr` 时改输出 stderr）。
- 默认配置打包在 QRC：`:/data/core/markly.json`（源文件 `src/data/core/markly.json`）。
- 便携模式：若可执行文件同目录存在 `markly_files/`、`user_files/` 文件夹，则用它们作为 app/user 配置目录。

## 架构（地基层）

- `MarklyApp`（= VNote 的 VNoteX）：中心单例协调器 + 全局信号枢纽（`src/core/marklyapp.*`）。
- `ConfigMgr` + `IConfig` 体系：MainConfig→{CoreConfig, EditorConfig, WidgetConfig} + 独立 SessionConfig（`src/core/`）。
- `Application`（QApplication 子类）：文件打开事件 + 主题文件夹热重载。
- `SingleInstanceGuard`：QLocalServer/Socket 单实例与 IPC。
- `CommandLineOptions`：QCommandLineParser。
- `Logger`：qInstallMessageHandler → 日志文件/stderr。
- `ThemeMgr`：**占位**，spec #2 替换为完整主题系统。
- `MainWindow`：**占位** QLabel 窗口，spec #3 替换为无边框 + QML 外壳。

## 后续 spec 的接入点（重要）

- `MarklyApp::getNotebookMgr/getBufferMgr/getTaskMgr()` 现返回 nullptr → 由 spec #4/#6/#16 安装真实 Manager。
- `MarklyApp` 的 Node/Event 相关信号在 spec #4/#5/#6 引入对应类型后补充。
- `CoreConfig/EditorConfig/WidgetConfig/SessionConfig` 仅含地基字段，后续 spec 增补（已在代码 TODO 注明）。
- `EditorConfig/WidgetConfig` 当前 round-trip 整段 JSON，避免后续字段丢失。
