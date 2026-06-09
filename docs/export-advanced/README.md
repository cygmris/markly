# Pandoc 自定义格式导出 — spec #15b

用系统 `pandoc` 把当前 Markdown 笔记导出为 docx / epub / latex / rst / org 等格式。复用 #16 的
QProcess 模式。参数构造是纯函数（可单测），实际转换经 QProcess **真实执行**（pandoc 已安装，
单测真正跑一次 markdown→html 转换）。

## 组成

| 文件 | 职责 |
| --- | --- |
| `src/core/export/pandocexporter.{h,cpp}` | `buildArgs`/`isAvailable`（纯）+ `exportTo`（真实 QProcess 转换） |
| `src/widgets/export/pandocbridge.{h,cpp}` | QML `Pandoc`：`available()`/`exportNote()` |
| `src/qml/shell/UnitedEntry.qml` | 命令「导出 Word/EPUB/LaTeX (Pandoc)」（仅 available 时列出） |

## PandocExporter

- `buildArgs(in, out, toFmt, fromFmt="markdown")` → `["-f", fromFmt, "-t", toFmt, "-o", out, in]`。
- `isAvailable()` → `QStandardPaths::findExecutable("pandoc")` 非空（能力探测，同 #20b 守卫）。
- `exportTo(markdown, outPath, toFmt)`：写 markdown 到 `QTemporaryFile`（.md）→
  `QProcess` 运行 pandoc（30s 超时）→ 成功 = 退出码 0 且输出文件非空；失败带 stderr。临时文件
  析构自动清理。

## 导出命令

统一入口仅当 `Pandoc.available()` 时追加「导出 Word (Pandoc)」(docx)、「导出 EPUB (Pandoc)」(epub)、
「导出 LaTeX (Pandoc)」(latex)；选择保存路径后 `Pandoc.exportNote(Views.currentText, path, fmt)`，
notify 结果。

## 验证

- **单元测试** `tests/test_pandoc.cpp`（真实执行 pandoc，能力守卫 `QSKIP`）：
  - `buildArgs("a.md","b.docx","docx")` == `{-f,markdown,-t,docx,-o,b.docx,a.md}`；
  - **真实转换** `exportTo("# Title\n\nHello world", out.html, "html")` → true，输出 HTML 含
    `<h1` 与 `Title`；
  - 失败路径（不可写目录）→ false（不崩）。
  - **ctest 24/24 全绿**（test_pandoc 真实跑 markdown→html）。
- 构建通过；UnitedEntry qmllint 无语法错误；offscreen 启动无错。
- 依赖：系统 `pandoc`（Arch 包 `pandoc-cli`）。未装时命令不列出、`exportTo` 返回「pandoc not found」。

## 后续小迭代（#15c export-merge）

全合一/整本合并导出（多笔记拼接）、wkhtmltopdf PDF、导出预设配置（模板/CSS/参数）、目录页、
docx 引用模板。
