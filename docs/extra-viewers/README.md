# Markly — 额外查看器（extra-viewers spec #18）

当打开的节点是 `.pdf` / `.html` 时，编辑区显示对应只读查看器（QWebEngine）而非 Markdown 编辑器。复用 #9 WebEngine、#6 ViewArea/标签。

## 再规划：思维导图拆 #18b

vnote 的 MindMap（emind 思维导图）是交互式图形编辑器（非只读），拆出 **#18b mindmap-viewer**。本 spec 只做 PDF/HTML 只读查看器。

## 组件

- **ViewArea.splits().currentPath**：每个 split 增当前激活 buffer 的绝对路径，供查看器 `file://` URL。
- **ViewerPane.qml**：`WebEngineView { settings.pdfViewerEnabled: true; settings.localContentCanAccessFileUrls: true; url: "file://"+path }`。PDF 用 WebEngine 内建 PDF 查看器；HTML 直接渲染。**页面 origin = 文件本身（file://），无跨源限制**（区别于 #10 预览页是 qrc origin 需 data URI）。
- **EditorArea 按类型切换**：`viewerType` 据当前标签扩展名（`/\.pdf$/i`→pdf、`/\.html?$/i`→html、否则 ""）；`MarkdownEditor`/分隔线/`PreviewPane` 仅 `viewerType===""` 时显示，否则显示 `ViewerPane`（`path: split.currentPath`）。

## 验证

- ctest 17/17（`test_bufferview` 加 `currentPath` 断言）。
- 截图 `/tmp/markly-shots/viewer18.png`：打开 `报告.html` → 编辑区 **ViewerPane 直接渲染该 HTML**（自定义 CSS：绿色标题、卡片边框、code 背景、列表），而非 markdown 编辑器。证明按扩展名切查看器 + file:// 加载生效。
- PDF（`pdfViewerEnabled`）同理；offscreen PDF 渲染不确定，HTML 作主验证（file:// 加载机制一致）。

## 后续（再规划）

- **#18b mindmap-viewer**：思维导图查看/编辑（emind 或等价）。
- 文本查看器只读模式、PDF 工具栏（页码/缩放）、查看器⇄编辑器切换按钮 → 待办。
