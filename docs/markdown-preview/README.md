# Markly — Markdown Preview（markdown-preview spec #9）

Markdown 渲染预览：QtWebEngine 的 QML `WebEngineView` 加载内置 `preview.html`(markdown-it 渲染)，编辑 / 阅读 / 分栏 三模式。

## 架构决策（已验证）

原技术栈「QWebEngine 预览」。本应用 UI 全在 QQuickWidget(QML)。采用 **QtWebEngine 的 QML `WebEngineView`**——**已验证 Qt 6.8 下 QQuickWidget(RHI) 能承载 WebEngineView 并正常渲染**（split 模式截图见下），**无需 QWidget 回退**。`main` 在 `QApplication` 构造前调 `QtWebEngineQuick::initialize()`（必须最先）。

## 组件

- **web 资源**（`src/data/web/`，QRC `:/data/web/`）：`markdown-it.min.js`(复用源 vnote-3201) + `preview.html`(div#content + JS `mdRender(text)`/`mdSetTheme(json)`) + `preview.css`(CSS 变量驱动主题:--bg/--text/--heading/--code-bg/--code-ink/--inline-code/--link/--quote/--border/--accent + 字体)。markdown-it 配置 `html:false`(安全)、linkify、typographer。
- **PreviewPane.qml**：`WebEngineView{ url: qrc:/data/web/preview.html }`；load 成功→`mdSetTheme`+`mdRender`；content 变→防抖 150ms→`runJavaScript("mdRender(JSON)")`；`Theme.themeChanged`→`mdSetTheme`。颜色用 hex(c) 把 QML color→#rrggbb(避免 ARGB/RGBA 顺序坑)。
- **ViewArea viewMode**（#6 扩展）：Q_PROPERTY viewMode(edit/read/split) + cycleViewMode(edit→split→read→edit)/setViewMode，持久化 SessionConfig view_mode。
- **EditorArea**（按 viewMode 组合）：edit→MarkdownEditor；read→PreviewPane；split→左右各半 MarkdownEditor + PreviewPane(中间分隔线)。预览 content 绑 split.currentText(实时)。
- **EditorToolbar eye** → cycleViewMode；save → saveTab。

## 验证

- ctest 8/8（含 viewMode cycle）。
- 截图 `/tmp/markly-shots/preview-grab.png`(split 模式)：左=语法高亮源码+行号，右=**markdown-it 渲染的富文本**(H1 标题主题绿/段落/...)。**WebEngineView 在 QQuickWidget 渲染成功**(offscreen 下也渲染了，Vulkan 不可用回退软件渲染)。

## 主题注入坑

QML color→CSS：QML `color.toString()` 给 "#aarrggbb"(ARGB)，Chromium 要 "#rrggbb"/"#rrggbbaa"(RGBA)，顺序不同。PreviewPane 用 `hex(c)` 从 c.r/c.g/c.b 构造 "#rrggbb" 规避。

## 后续（再规划）

- **新增 spec #9b preview-extras**：MathJax(数学公式)、Mermaid/Flowchart/PlantUML/Graphviz(图表)、Prism(代码块语言级高亮)、TOC/章节号、outline 大纲提取、user.css。这些体量大，从 #9 拆出单列。已加入 roadmap。
- 双向滚动同步(编辑↔预览)、图片本地路径解析、PDF/HTML 导出复用预览渲染 → 相应 spec。
- B 沉浸写作风格默认偏 read/split + 衬线正文 → 可在 polish 调。
