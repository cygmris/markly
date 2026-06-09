# Changes

## Unreleased

### buffer-view-area (spec #6) — 2026-06-09
* 打开/视图层：Buffer/BufferMgr(按路径去重) + ViewArea 桥接(splits/tabs/currentText + 分屏) + EditorArea.qml(共享编辑区)。
* 接住 #5 openFileRequested → 标签打开笔记，可编辑文本占位(#7 换编辑器内核)、保存、脏标记、基础左右分屏。
* 会话恢复：SessionConfig opened_files/current_file，重启恢复上次标签。
* MarklyApp.getBufferMgr() 返回真实(替换 #1 nullptr)；三套外壳编辑区嵌入 EditorArea 显示真实内容。
* test_bufferview(去重/dirty/save/标签/分屏/会话)；ctest 7/7；截图验证(预开两文件→两标签+真实内容)。

### node-explorer-ui (spec #5) — 2026-06-09
* 笔记本数据层接到三套外壳：真实笔记本树替换占位（A 树/B 笔记列表/C 笔记本导航）。
* NotebookExplorer 桥接（QObject context property，visibleNodes 展平树 + Q_INVOKABLE 交互）+ DialogHelper（原生对话框）。
* 节点 CRUD（右键菜单：新建笔记/文件夹、重命名、删除回收站/永久、颜色、外部导入）经 #4 Notebook 门面三处一致。
* 笔记本选择/新建/打开/切换；空状态 EmptyState；外部文件显示 + Notebook.importNode 导入。
* 确立「占位→真实 model」桥接范式（core→QObject→QML），后续 UI spec 复用。
* test_notebookexplorer（树展平/CRUD/颜色/外部/导入）；ctest 6/6；三套外壳真实树截图验证。

### notebook-core (spec #4) — 2026-06-09
* 笔记本数据层（纯 core）：磁盘格式 vx_notebook.json + 每目录 vx.json + SQLite 三表(node/tag/tag_node)，严格兼容 VNote v3.x。
* INotebookBackend/LocalNotebookBackend、NotebookConfig、vx_node_config 序列化、Node/NodeVisual、NotebookDatabaseAccess、NotebookTagMgr、DummyVersionController、Notebook(CRUD 门面)、NotebookMgr。
* 节点 CRUD 三处双写（磁盘→vx.json→SQLite）：新建/重命名/移动回收站/永久删除/颜色自定义/标签；懒加载子文件夹；node.id 由 DB autoincrement 回写 vx.json。
* NotebookMgr 多笔记本 + 当前 + 持久化(SessionConfig)，接入 MarklyApp::getNotebookMgr()（替换 spec#1 nullptr 占位）。
* 架构精简：单实现接口合并（VXNode→Node、BundleNotebook/configmgr→Notebook、工厂→NotebookMgr）。ExternalNode 延后 #5。
* test_notebookcore 5 例（三处一致 + 重开懒加载 + VNote v3 兼容样例读取）；ctest 5/5。

### app-shell-ui (spec #3) — 2026-06-09
* 无边框自绘主窗口 FramelessMainWindow（startSystemMove/Resize，QML 经 Win.* 控制）。
* QML 外壳 MarklyShell：Loader 按 Appearance.style 切三套布局 ShellRefined(A)/ShellFocus(B)/ShellWorkbench(C)，1:1 复刻设计稿整窗。
* 共享子组件（WinControls/RailButton/StatusBar/Chip/EditorTab/EditorToolbar/TreeRow/OutlineRow/NoteCard/Omnibar）只引用 Theme.* 角色。
* 图标系统：IconPaths.js（~50 图标）+ Icon.qml（QtQuick.Shapes 描边渲染，随主题着色/缩放）。
* 外观面板接入（右下角「外观」抽屉）；风格/主题/强调色/布局整窗实时切换。
* MainWindow 继承 FramelessMainWindow 承载 MarklyShell，替换 spec #1/#2 占位；MARKLY_SHOT 截图 hook。
* 验证：build 通过；三风格 × 浅/深 × 强调色 offscreen 截图渲染贴近设计稿。

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
