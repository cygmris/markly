# Changes

## Unreleased

### app-foundation (spec #1) — 2026-06-09
* 建立 Qt 6.8 + CMake/Ninja 工程骨架（C++17，AUTOMOC/UIC/RCC）。
* `MarklyApp` 单例协调器（对标 VNote 的 VNoteX）+ 全局信号枢纽。
* `ConfigMgr` 分层配置系统（IConfig + MainConfig/CoreConfig/EditorConfig/WidgetConfig/SessionConfig），磁盘 JSON 结构对齐 VNote、读兼容；默认配置打包 QRC。
* `Application`（文件打开事件 + 主题热重载）、`SingleInstanceGuard`（单实例 + IPC）、`CommandLineOptions`、`Logger`。
* `ThemeMgr`/`MainWindow` 占位（分别由 spec #2、#3 替换）。
* 单元测试：configmgr / commandlineoptions / singleinstanceguard（ctest 3/3 通过）。
* 应用可构建、可启动、可干净退出；首次运行生成 `~/.config/Markly/Markly/{markly.json,session.json,markly.log}`。
