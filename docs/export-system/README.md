# Markly — 导出（export-system spec #15）

把当前笔记导出为 **Markdown / HTML / PDF**，复用 #9 的 QWebEngine + markdown-it 预览渲染、#10 的图片 data URI 内联。无外部依赖（Pandoc/wkhtmltopdf 拆 #15b）。

## 组件

- **ExportHelper**（`widgets/export/`，context property `Export`，markly_core）：`writeText(path, text)`（UTF-8 写）、`readResource(":/...")`（读 qrc 资源，用于内联 CSS）。
- **DialogHelper.saveFile**（title, defaultName, filter）：原生保存对话框。
- **ViewArea.currentText**（Q_PROPERTY）：当前激活笔记内容（MD 导出 + 渲染源）。
- **ExportView.qml**（隐藏 WebEngine，A4 像素 794x1123）：加载 `preview.html`；`resolveContent`（本地图片→data URI，同 PreviewPane）、`pushTheme`（含 Prism token）、`exportTo(fmt, content, baseDir, path, cb)`：
  - **PDF**：`mdRender` 后 `web.printToPdf(path)`，`onPdfPrintingFinished` 回报。
  - **HTML**：`mdRender` 后取 `document.documentElement.outerHTML` → **inline preview.css（readResource 替换 `<link>`）+ 删除 `<script>` 标签** → 自包含写盘。
- **EditorToolbar 导出菜单**：「导出」按钮 → Menu(Markdown/HTML/PDF) → `Dialogs.saveFile`（默认名=笔记名+扩展）→ MD `Export.writeText` / HTML·PDF `ExportView.exportTo` → `Dialogs.notify` 提示。

## 自包含 HTML 的关键处理

QWebEngine `outerHTML` 默认保留对 `preview.css` / markdown-it·Prism 脚本的**外部相对引用**（重开即失效），且 PDF 内图片需内联。处理：
- 图片：渲染前预解析为 `data:` URI（#10 复用）。
- CSS：`readResource(":/data/web/preview.css")` 内联为 `<style>`（主题 CSS 变量已是 `<html>` 的内联 style，随 outerHTML 保留）。
- 脚本：渲染后内容已在 DOM，`<script>` 不再需要 → 正则删除。
→ 导出的 HTML 离线单文件可正确打开。

## 验证

- ctest 15/15（新增 `test_export`：writeText 往返 + 坏路径 false）。
- 命令行 `MARKLY_EXPORT="fmt:path"`（离线 hook）导出 seed 笔记：
  - HTML（25KB）自包含 —— 无外部 src/href、含 `<style>`、`#content` 渲染内容、`language-cpp`(Prism)、`data:image/png`(内联图)、标题。
  - PDF —— `file` 判定 `PDF document, version 1.4, 1 page`（offscreen `printToPdf` 亦可生成）。
- offscreen 退出 WebEngine segfault(139) 仍为 #9 既有现象，文件已先写。

## 后续（再规划）

- **#15b export-advanced**：Pandoc 自定义格式、wkhtmltopdf、全合一（合并多笔记/整本）、打印、导出选项（纸张/页边距/是否内联）。
- 批量导出、导出进度、导出后打开目录 → 待办。
