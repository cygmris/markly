# 全合一/合并导出 — spec #15c

把一个目录下的多个 Markdown 笔记合并成单一文档，再导出（复用 #15b PandocExporter 真实 pandoc，
或直接写 .md）。合并逻辑为纯函数（可单测），整条链路用真实 pandoc 验证。

## 组成

| 文件 | 职责 |
| --- | --- |
| `src/core/export/notemerge.{h,cpp}` | `merge`（拼接）/`mergeDir`（遍历目录）/`exportMergedDir`（合并+导出） |
| `src/widgets/export/mergebridge.{h,cpp}` | QML `Merge`：`exportDir(dir, out, format)` |
| `src/qml/shell/UnitedEntry.qml` | 命令「合并导出 Markdown/HTML/Word（当前文件夹）」 |

## NoteMerge

- `merge(notes, includeTitles)`：每篇（可选 `# 标题` + 内容）用 `\n\n---\n\n` 连接。
- `mergeDir(dirPath)`：`QDir.entryList("*.md", Files, Name)` 按文件名排序，逐个读 UTF-8，
  标题=文件名去 `.md`，再 `merge`；无 `.md` → `""`。
- `exportMergedDir(dir, out, format)`：`md = mergeDir`，空→失败；`format=="md"` 直写；否则
  `PandocExporter::exportTo`（真实 pandoc）。

## 导出命令

统一入口「合并导出 Markdown」（md，直写）、「合并导出 HTML」（html，pandoc）、「合并导出 Word」
（docx，pandoc）——后两者仅 `Pandoc.available()` 时列出。命令合并 `Views.currentFileDir`（当前
笔记所在文件夹）的所有 `.md`，`Dialogs.saveFile` 取路径 → `Merge.exportDir` → notify。

## 验证

- **单元测试** `tests/test_merge.cpp`（纯逻辑 + 真实 pandoc 守卫）：
  - `merge` 两篇 → 含 `# A`/x/`---`/`# B`/y；
  - `mergeDir`：临时目录 `1.md`/`2.md` → 含两者且按名序（Alpha 在 Beta 前）；空目录 → `""`；
  - `exportMergedDir` md 直写 → 文件存在；**真实** html（pandoc）→ 输出同时含 Alpha 与 Beta。
  - **ctest 25/25 全绿**（test_merge 含真实 pandoc 合并→html）。
- 构建通过；UnitedEntry qmllint 无语法错误；offscreen 启动无错。

## 后续小迭代（#15d export-presets）

wkhtmltopdf PDF、导出预设（pandoc 模板/CSS/参数）、目录页/书签、整本递归（含子文件夹）、
docx 引用模板、合并顺序自定义。
