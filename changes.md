# Changes

## Unreleased

### theme-appearance (spec #2) — 2026-06-09
* 语义 token 体系（ThemeTokens）+ 三套风格 A/B/C × 浅深 × 强调色生成（ThemeModel，逐值复刻设计稿）。
* Appearance 偏好（style/theme/accent/showLeft/showRight/density）+ 即时持久化到 WidgetConfig.appearance + theme=auto 跟随系统。
* ThemeMgr 重写为 QML 桥接（全部 token 暴露为 Q_PROPERTY）+ StyleSheetGenerator（token→QWidget QSS），替换 spec #1 占位。
* 引入 Qt Quick；QML 外观面板 AppearancePanel.qml + 校验载体 ThemePreview.qml（占位 MainWindow 用 QQuickWidget 承载）。
* 单测 test_thememodel：token 精度 + accent 派生 + 持久化往返（ctest 4/4 通过）；QML 载入无错。

### app-foundation (spec #1) — 2026-06-09
* 建立 Qt 6.8 + CMake/Ninja 工程骨架（C++17，AUTOMOC/UIC/RCC）。
* `MarklyApp` 单例协调器（对标 VNote 的 VNoteX）+ 全局信号枢纽。
* `ConfigMgr` 分层配置系统（IConfig + MainConfig/CoreConfig/EditorConfig/WidgetConfig/SessionConfig），磁盘 JSON 结构对齐 VNote、读兼容；默认配置打包 QRC。
* `Application`（文件打开事件 + 主题热重载）、`SingleInstanceGuard`（单实例 + IPC）、`CommandLineOptions`、`Logger`。
* `ThemeMgr`/`MainWindow` 占位（分别由 spec #2、#3 替换）。
* 单元测试：configmgr / commandlineoptions / singleinstanceguard（ctest 3/3 通过）。
* 应用可构建、可启动、可干净退出；首次运行生成 `~/.config/Markly/Markly/{markly.json,session.json,markly.log}`。
