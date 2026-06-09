# 导出预设/选项 — spec #15d

给 Pandoc 导出加 **独立文档（standalone）/ 目录页（TOC）/ 自定义 CSS** 选项。选项→参数构造是
纯函数（可单测），实际转换用真实 pandoc（`--toc` 产出目录，验证 `<nav id="TOC">`）。这是导出族
的收口 spec。

## 组成

| 文件 | 职责 |
| --- | --- |
| `src/core/export/pandocexporter.{h,cpp}` | `PandocOptions{standalone,toc,cssPath}` + 选项版 `buildArgs`/`exportTo` |
| `src/widgets/export/pandocbridge.{h,cpp}` | `exportNoteOpts(...)` |
| `src/qml/shell/UnitedEntry.qml` | 命令「导出 HTML（含目录）」（standalone+toc） |

## 选项

- 选项版 `buildArgs`：基础参数（`-f markdown -t fmt -o out in`）后追加
  `--standalone`（独立 HTML 带 `<head>`）、`--toc`（目录页）、`--css <path>`（自定义样式）。
- `exportTo(markdown, out, fmt, opts)`：同 #15b（临时 md + QProcess），用选项版 buildArgs。
  无选项版保留兼容 #15b/#15c（委托默认 opts）。
- 命令「导出 HTML（含目录）」= `exportNoteOpts(text, path, "html", standalone=true, toc=true, css="")`，
  仅 `Pandoc.available()` 列出。

## 验证

- **单元测试** `tests/test_pandoc.cpp`（扩展，真实 pandoc）：
  - 选项版 `buildArgs({standalone,toc,css})` 含 `--standalone`/`--toc`/`--css`/`a.css`；空选项不含；
  - **真实转换** `exportTo("# A\n## B\n## C", out.html, "html", {standalone,toc})` → true，输出含
    `id="TOC"`（pandoc 目录）。
  - **ctest 26/26 全绿**（test_pandoc 4 用例，含真实 --toc）。
- 构建通过；UnitedEntry qmllint 无语法错误；offscreen 启动无错。

## 不支持 / 可选

- **wkhtmltopdf**：已从 Arch 仓库移除（包不存在），本项目不支持该 HTML→PDF 路径；内建 PDF 走
  **QWebEngine `printToPdf`**（#15）。
- **LaTeX PDF**（`pandoc -o x.pdf`）：需 `texlive` 引擎（未装）；安装 texlive 后 `exportNote(..., "pdf")`
  可经 pandoc 生成 LaTeX PDF（可选）。
- 导出预设的更多 pandoc 参数（模板 `--template`、元数据）可后续按需在 PandocOptions 扩展。
