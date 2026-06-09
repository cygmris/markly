# Changes

## Unreleased

### export-system (spec #15) — 2026-06-09
* 导出当前笔记为 Markdown/HTML/PDF：编辑器工具栏「导出」菜单→保存对话框→生成文件。复用 #9 预览渲染+#10 图片 data URI。
* ExportHelper(context property Export)writeText/readResource;DialogHelper.saveFile;ViewArea.currentText。
* ExportView.qml(隐藏 WebEngine A4):PDF 用 printToPdf;HTML 取 outerHTML→inline preview.css+删 script→自包含;图片渲染前 data URI 内联。
* 自包含 HTML:图片内联+CSS 内联(readResource)+主题变量(html inline style)+删脚本→离线单文件可开。
* ctest 15/15(新增 test_export)；MARKLY_EXPORT 离线验证 HTML(自包含)+PDF(PDF-1.4 1 页)生成。
* 再规划：Pandoc/wkhtmltopdf/全合一/打印拆出 #15b export-advanced。

### snippet-template (spec #14) — 2026-06-09
* 代码片段/魔法词/模板：左 dock「片段」页(列表+插入+删除+新增)，插入展开到编辑器光标；右键文件夹「从模板新建笔记」。
* SnippetMgr(core):片段存 <config>/snippets/*.json;apply 展开魔法词(%date%/%time%/%datetime%/%note%/%uuid%)+$$ 选区+@@ 光标(移除记 offset);seedDefaults(日期/代码块)。
* SnippetBridge(context property Snippets)list+apply/add/remove;ViewArea.requestInsert→insertText 信号→MarkdownEditor 光标插入定位(同 #9b 模式)+currentFileName。
* NotebookExplorer.newNoteFromTemplate(展开模板写入新笔记);三外壳 A/B snippet 按钮→leftPage=snippet。
* ctest 14/14(新增 test_snippet)；截图验证片段页内置代码块/日期+按钮。
* 再规划：脚本型片段/快捷键绑定/模板选择器对话框 → 后续小迭代。

### quickaccess-history-flash (spec #13) — 2026-06-09
* 快速访问/历史/闪念：左 dock「快速访问」页(已固定 + 最近 MRU)，右键笔记「固定到快速访问」，「快速记录」一键打开闪念 flash.md。
* HistoryMgr(core,markly_core):addToHistory(MRU 去重+cap 50)/pin/unpin/isPinned/ensureFlashFile(AppConfigLocation/flash.md);MarklyApp connect openFileRequested→addToHistory(所有打开来源进历史)。
* SessionConfig 加 history/quick_access 持久化;QuickBridge(context property Quick)history/quickAccess+open/pin/unpin/isPinned/openFlash;NotebookExplorer.nodeAbsPath。
* QuickAccessPanel.qml(已固定/最近两区+空状态);三外壳 history 按钮/「最近」nav→leftPage=quick;A「快速记录」→openFlash。
* ctest 13/13(新增 test_history)；截图验证快速访问页固定+最近列表。
* 再规划：QuickAccess 文件夹/历史时间戳/Flash 多页 → 后续小迭代。

### images-local (spec #10) — 2026-06-09
* 本地图片：编辑器 Ctrl+V 粘贴 / 拖入图片 → 存到笔记 vx_images/ → 插入 ![](rel) → 预览内联显示。
* ImageHelper(context property Images) clipboardHasImage/pasteImage/importImage/toDataUri；ViewArea.currentFileDir(Q_PROPERTY)。
* MarkdownEditor Ctrl+V(仅剪贴板有图才拦)+DropArea(text/uri-list)；PreviewPane resolveContent 渲染前把本地图片预解析为 base64 data URI。
* 关键决策：data URI 而非 file://——QWebEngine qrc 页跨源禁止加载 file:// 图片(设置/flag 均无效),内联 data URI 与渲染模式无关;currentFileDir 由 Q_INVOKABLE 改 Q_PROPERTY(NOTIFY)才随视图刷新(图片不显示真正根因)。
* ctest 12/12(新增 test_images)；截图验证预览内联显示本地图片。
* 再规划：图床上传拆出 #10b imagehost-upload(GitHub/Gitee+token 配置)；GIF/imsize 等待办。

### preview-extras (spec #9b) — 2026-06-09
* 预览渲染增强：markdown-it 插件(footnote/sub/sup/mark/emoji/task-lists/anchor/toc,防御式 use)+ Prism 代码块语法高亮(token 配色随主题 CSS 变量)。复用源 web 资源。
* 真实大纲：ViewArea 解析当前 buffer 标题(跳过围栏代码块内 #)→ Q_PROPERTY outline；三外壳大纲面板接真实数据(替换 DemoData)，点击经 gotoLineNow 即时跳转(补 #8 已激活 buffer 跳转缺口)。
* PreviewPane pushTheme 增注 Prism token 颜色(Theme 角色→hex)。
* ctest 11/11(test_bufferview 加 outline 用例)；截图验证任务清单复选框+Prism 高亮+真实大纲。
* 再规划：重型引擎拆出 #9c diagram-math-extras(MathJax/Mermaid/Flowchart/PlantUML/Graphviz/WaveDrom)；章节号/user.css/滚动同步→polish。

### tags-system (spec #11) — 2026-06-09
* 标签系统 UI：左 dock 标签页(标签 Chip+笔记数+点击筛选+选中标签的笔记列表)，复用 #4 数据层(tag/tag_node/NotebookTagMgr/updateNodeTags)+#12 queryNodesByTag。
* TagBridge(context property Tags) tags/selectedTag/taggedNodes + selectTag/openTaggedNode/refresh；监听 currentNotebookChanged + MarklyApp.tagsChanged 刷新。
* NotebookExplorer 加 nodeTagsCsv/setNodeTags；NodeContextMenu「标签…」(promptText 编辑逗号分隔)；三外壳 tag 按钮/标题切 leftPage=tags。
* 修复两个 #4 既有 Bug(被 #11 暴露)：(1) addTag INSERT OR REPLACE→OR IGNORE(避免 FK CASCADE 连带删 tag_node)；(2) open() tagMgr.load 早于 rebuildDatabase→rebuild 后重 load。
* ctest 11/11(新增 test_tags)；截图验证标签 chip+计数+筛选笔记列表。
* 再规划：标签父子层级树/管理(重命名删除)/拖拽打标 → polish 或后续小迭代。

### markdown-editor-input (spec #8) — 2026-06-09
* 编辑器输入辅助：自动缩进、列表续行(无序/有序/任务项,空标记清除)、括号/引号配对(含包裹选区/跳过/成对退格)、多行缩进反缩进、查找替换栏、跳转到行。
* 纯文本算法抽到可单测 EditInputHelper(core/editor/)；EditInputQml(context property EditInput) 暴露 QML；EditorConfig 加 auto_indent/continue_list/auto_pair。
* FindReplaceBar.qml(查找/替换/上下一个循环/Aa 大小写/全部替换/Esc)固定编辑器顶部；MarkdownEditor Ctrl+F/H/Esc + gotoLine。
* 接住 #12：Searcher 内容命中填 m_line(首个命中行)；SearchBridge.openResult→MarklyApp.gotoLineRequested→ViewArea pending→编辑器 onBufferIdChanged 后 takePendingGotoLine→gotoLine。
* ctest 10/10(新增 test_editorinput)；截图验证编辑器渲染清单/有序/代码/引用。
* 再规划：拆出 #8b vi-mode、#8c spell-check-hunspell(独立大子系统)；格式工具栏命令/已激活 buffer 跳转→polish。

### search-fts (spec #12) — 2026-06-09
* 全文搜索（技术栈关键点 SQLite FTS5 落地）：每笔记本 DB 建 `node_fts` FTS5 虚拟表索引 名称/路径/正文。
* 与源差异：源 FileSearchEngine 遍历文件无索引；本应用按技术栈用 FTS5 索引 + 单一同步 Searcher（精简 token/异步引擎）。
* Searcher 按 scope(笔记本/全部/文件夹/已打开) × object(名称/内容/路径/标签) 查询：内容 FTS5 MATCH+snippet、名称/路径内存匹配、标签查节点 tags；去重组装结果。
* SearchBridge(context property Search) results/state/count + search/openResult/clear；openResult→openFileRequested；监听 MarklyApp.noteSaved 增量 reindex。
* SearchPanel.qml：关键词+scope 段按钮+object toggle+结果列表(snippet `<mark>`→accent 高亮)；接入三外壳左 dock 搜索页(A 活动栏 / B 搜索框+活动栏 / C Omnibar)。
* ctest 9/9（新增 test_search：内容/名称/空词/增量）；截图验证搜索「桥接」命中+高亮 snippet+路径。
* 再规划：跳转到行→#8；重命名 name 索引更新/正则·magic-switch→polish；搜索历史→#13。

### markdown-preview (spec #9) — 2026-06-09
* Markdown 渲染预览：QtWebEngine 的 QML WebEngineView 加载内置 preview.html(markdown-it)，编辑/阅读/分栏三模式。
* 架构决策验证：QML WebEngineView 在 QQuickWidget(Qt6.8 RHI) 渲染成功，无需 QWidget 回退；main 在 QApplication 前 QtWebEngineQuick::initialize()。
* 复用源 markdown-it.min.js；preview.css 用 CSS 变量驱动主题(随 Theme 着色)；防抖渲染。
* ViewArea viewMode(持久化 SessionConfig) + EditorArea 按模式组合 MarkdownEditor+PreviewPane + 工具栏 eye 切换。
* ctest 8/8(含 viewMode cycle)；split 模式截图验证(左源码高亮+右渲染富文本)。
* 再规划：拆出新 spec #9b preview-extras(MathJax/Mermaid/Prism/TOC/outline)。

### markdown-editor-core (spec #7) — 2026-06-09
* 真正的 Markdown 源码编辑器：语法高亮 + 行号 + 当前行高亮 + Tab/缩进 + Ctrl+S + 真实行列/字数统计。
* 架构决策(spike 验证)：不嵌 QWidget(Wayland 不稳)，改用 QML TextEdit + C++ QSyntaxHighlighter 挂 QQuickTextDocument。
* MarkdownHighlighter(QML 类型 Markly.Editor，规则 QRegularExpression，颜色取 ThemeMgr，主题变重高亮) + EditorConfig 字段 + EditorCfg 桥接 + MarkdownEditor.qml。
* Views 统计(statsLine/Column/LineCount/CharCount)；三套外壳编辑区用 MarkdownEditor，A 状态栏真实统计。
* test_editorconfig；ctest 8/8；截图验证(标题/代码/列表/链接/引用/强调全高亮 + 行号 + 真实统计)。

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
