# Markly — Markdown Editor Core（markdown-editor-core spec #7）

把 #6 的纯文本 TextArea 占位升级为**真正的 Markdown 源码编辑器**：语法高亮 + 行号 + 当前行高亮 + Tab/缩进 + Ctrl+S + 真实行列/字数统计。

## 架构决策（重要）

原技术栈写「QWidget 编辑器混合」。**经评估并验证**：在 `QQuickWidget` 承载的 QML 窗口里用 `createWindowContainer` 嵌入 QWidget 编辑器在 Wayland 下不稳定（foreign window 裁剪/层叠/焦点）。Qt6 更稳健的等价方案——也是本 spec 采用并跑通的——是：

> **QML `TextEdit` + C++ `QSyntaxHighlighter` 挂到其 `QQuickTextDocument::textDocument()`**

既得真正的语法高亮，又规避 QWidget-in-QML 嵌入风险。这是"QWidget 编辑器内核"在 QML 宿主下的落地形态。截图验证：A 编辑区显示完整语法高亮 + 行号 + 真实统计。

## 组件

- **MarkdownHighlighter**（`src/widgets/editors/`，QObject，QML 类型 `Markly.Editor`）：`Q_PROPERTY(QQuickTextDocument* document)`，setter 把内部 `MdHighlighter`(QSyntaxHighlighter) 挂到 `document.textDocument()`。规则(QRegularExpression)：ATX 标题/粗/斜/行内代码/围栏代码块(block state)/链接/引用/列表标记/删除线/mark。颜色取自 ThemeMgr(heading/emphasis/codeInline/link/dim/accent/codeInk)，`ThemeMgr::themeChanged`→重高亮。
- **EditorConfig**（core，#1 增补字段）：font_size/tab_width/expand_tab/line_number/highlight_current_line，持久化 markly.json editor 段。**EditorCfgQml**(context property `EditorCfg`) 暴露给 QML。
- **MarkdownEditor.qml**：Flickable 内 Row[行号 gutter(单个多行 Text，同字体故对齐) + TextEdit(等宽/无换行/IME)]；当前行高亮(cursorRectangle)；挂 MarkdownHighlighter；Tab→缩进、Shift+Tab、Ctrl+S→saveTab；统计→Views.setStats。EditorArea 用它替换 TextArea。
- **Views 统计**（ViewArea 扩展）：statsLine/statsColumn/statsLineCount/statsCharCount + setStats；A/C 状态栏绑定(替换占位)。+ `textForBuffer(id)` 供编辑器读取内容。

## 关键坑（写给后续）

- **QML 绑定求值顺序导致内容空白**：MarkdownEditor 的 `onBufferIdChanged: edit.text = content` 中，`content` 绑定此刻可能尚未重算（仍是旧值"")，导致编辑器空白。修复：用 `Views.textForBuffer(bufferId)` 直接从 C++ 读最新内容，绕过 QML 绑定求值时序。
- **行号对齐**：gutter 用「一个多行 Text(1\n2\n3…)」而非每行一个 Text，与 TextEdit 同字体同 pixelSize 即自动对齐。
- **`QQuickTextDocument*` 作 Q_PROPERTY**：头文件必须 include `<QQuickTextDocument>`（完整类型），前向声明会触发 moc 的 metatype 静态断言失败。
- TextEdit text 不绑定 currentText，仅 buffer 切换时 set（防循环，承 #6）。

## 验证

- `tests/test_editorconfig.cpp`（ctest 通过）：默认值 + 更新 + toJson 往返。
- 截图 `/tmp/markly-shots/editor-syntax2.png`：打开含标题/代码/列表/链接/引用/强调的 md → 完整语法高亮(标题绿/行内码紫/引用灰斜/列表标记绿/强调橙斜/代码块/链接蓝) + 行号 1-22 + 状态栏"行 1,列 1 · 22 行 · 330 字"。

## 后续

- **#8 markdown-editor-input**：Vi 模式、VSCode 输入模式、查找替换、自动缩进/列表/括号、拼写检查(Hunspell)。
- **#9 markdown-preview**：QWebEngine 渲染预览(markdown-it/MathJax/Mermaid/代码高亮)，与编辑器并排/切换。
- 工具栏格式按钮(EditorToolbar)接编辑命令(加粗/标题等)留 #8。
- 围栏代码块内的语言级语法高亮(c++/python…)可后续增强(目前整块 codeInk 色)。
