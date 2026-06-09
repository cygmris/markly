# Markly — 编辑器输入与编辑辅助（markdown-editor-input spec #8）

在 #7 markdown-editor-core 之上补全编辑器输入辅助：自动缩进、列表续行、括号/引号配对、多行缩进/反缩进、查找替换栏、跳转到行（接住 #12 搜索结果跳转）。

## 再规划：拆出两个独立大子系统

vnote 编辑器输入里两个**独立大子系统**体量过大，按「避免巨型 spec + 先规划再实现」拆出单列（已入 roadmap）：
- **#8b vi-mode**：Vi/Vim 输入层（有状态键盘子系统）。
- **#8c spell-check-hunspell**：Hunspell 拼写检查（新依赖 + 词典 + 波浪线 + 建议菜单）。

本 spec 只做与 QML TextEdit 直接相关、无新依赖的编辑辅助。

## 组件

- **EditInputHelper**（`core/editor/editinputhelper.{h,cpp}`，纯函数可单测）：
  - `continueOnEnter(curLine, autoIndent, continueList)` → `{clearMarker, lineIndent, insert}`：解析无序 `-/*/+`、有序 `N./N)`、任务 `- [ ] `、缩进；空标记行返回 clearMarker。
  - `indentLines/outdentLines(block, tabUnit)`：多行整体缩进/反缩进（反缩进吃一个 `\t` 或最多 tabUnit 个前导空格）。
  - `pairFor(ch)`：`(`/`[`/`{`/`"`/`` ` `` 的右半。
- **EditInputQml**（`widgets/editors/editinputqml.{h,cpp}`，context property `EditInput`）：把上述函数暴露给 QML（continueOnEnter 返回 QVariantMap）。
- **EditorConfig / EditorCfgQml**：新增 `auto_indent`/`continue_list`/`auto_pair`（默认开，VNote 风格键名，读写持久化），经 EditorCfg 暴露给 QML。
- **MarkdownEditor.qml**（扩展 `Keys.onPressed`）：
  - 回车 → `EditInput.continueOnEnter`，clearMarker 则清标记，否则插入 `换行+前缀`。
  - 配对键 → 插成对 + 光标置中；有选区则包裹；键右半且光标右侧即该字符则跳过；成对空括号间退格删两半（仅 autoPair 开）。
  - 跨行选区 Tab / Shift+Tab(Backtab) → `reindentSelection` 多行缩进/反缩进；无选区 Tab 维持 #7 插入缩进。
  - Ctrl+F/Ctrl+H → 打开查找/替换栏；Esc 关闭。
  - `gotoLine(n)`：定位第 n 行行首 + 聚焦 + 滚动居中。
- **FindReplaceBar.qml**（`qml/shell/components/`）：查找框 + 上/下一个（循环）+ `m/n` 计数 + Aa 大小写 + 替换框 + 替换/全部替换 + 关闭；纯 JS 作用于 `target.text`/选区，全部替换一次性 set text 触发 buffer 同步。固定在编辑器可见顶部（`y: contentY`）。

## 跳转到行（接住 #12）

- Searcher 内容命中现填 `m_line`（读文件内容算关键词首次出现行，1 基）。
- 链路：`SearchBridge.openResult` → 先 `MarklyApp::gotoLineRequested(line)`（ViewArea 存 `m_pendingGotoLine`）→ 再 `openFileRequested(path)`（打开/激活）→ MarkdownEditor `onBufferIdChanged` 后 `Views.takePendingGotoLine()`，有则 `Qt.callLater(gotoLine)`。
- 已知限制：若点击的结果就是当前已激活 buffer（bufferId 不变），onBufferIdChanged 不触发，则不跳转（边角，列入 polish）。

## 验证

- ctest 10/10（新增 `test_editorinput`：续行 无序/有序/任务/空标记清除/普通缩进/续行关闭、indent/outdent（空格与 `\t`）、pairFor、Searcher 行号=3）。
- 截图 `/tmp/markly-shots/editor8.png`：打开含任务清单/有序列表/代码块/引用的笔记，语法高亮 + 行号 + 真实统计（21 行 187 字）正常；编辑辅助逻辑由单测覆盖。
- offscreen 退出 WebEngine segfault(139) 仍为 #9 既有问题，截图先存不受影响。

## 待办 / 后续

- 格式工具栏命令（加粗/斜体/标题按钮实际插入语法）—— #3 工具栏已在，命令接线小，留作本模块 polish 或并入 #19 设置后。
- Vi 模式 → **#8b**；拼写检查 → **#8c**；VSCode 完整键位映射 → 视需要在 #8b 或 polish。
- 跳转到当前已激活 buffer 的行 → polish。
