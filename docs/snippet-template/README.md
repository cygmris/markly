# Markly — 代码片段 / 魔法词 / 模板（snippet-template spec #14）

实现 代码片段（Snippet）+ 魔法词（Magic Word）+ 笔记模板（Template）。复用 #5 桥接范式、#6 ViewArea、#9b 信号插入模式、#12 `leftPage`。

## 组件

- **SnippetMgr**（`core/snippet/`，markly_core）：
  - 片段存 `<AppConfigLocation>/snippets/<name>.json`（键 `content/description/cursor_mark/selection_mark`）。
  - `load()`：空目录则 `seedDefaults`（内置「日期」`%date%`、「代码块」围栏含 `@@`/`$$`）。
  - `add/remove/get/snippets`；`apply(name, selectedText, noteName, &cursorOffset)`：展开魔法词 → `$$`→选中文本 → `@@`→光标位置（移除并记 offset，无则末尾）。
  - `expandMagicWords`：`%date%`/`%time%`/`%datetime%`/`%note%`/`%uuid%`，未知保留。
- **SnippetBridge**（`widgets/snippet/`，context property `Snippets`）：`Q_PROPERTY list`（`[{name,description}]`）；`apply→{text,cursorOffset}`/`add/remove`。
- **ViewArea**：`requestInsert(text, cursorOffset)` → 信号 `insertText` → `MarkdownEditor` Connections 在光标插入并定位（同 #9b `gotoLineNow` 模式）；新增 `Q_PROPERTY currentFileName`（`%note%` 用）。
- **SnippetPanel.qml**：列表（name + description）+「插入」（`Snippets.apply` → `Views.requestInsert`）+「×」删除 +「新增」（promptText 名称/内容）。
- **从模板新建笔记**：`NotebookExplorer.newNoteFromTemplate(parentId, name, templateName)` —— `newNode` 后把展开后的模板内容写入新笔记文件；`NodeContextMenu「从模板新建笔记…」`（promptText 模板名 + 笔记名）。
- **三外壳**：A/B snippet 活动栏按钮 → `leftPage="snippet"`；左 dock 嵌 `SnippetPanel`。C 也嵌面板（可达）。

## 验证

- ctest 14/14（新增 `test_snippet`：魔法词展开 + 未知保留 + `%date%` 形如 YYYY-MM-DD、apply 标记 `pre$$mid@@post`+sel="X"→`preXmidpost`/offset、无 `@@`→末尾、CRUD 往返）。
- 截图 `/tmp/markly-shots/snippet14.png`：片段页列出内置「代码块」「日期」+ 插入/删除/新增按钮。
- offscreen 退出 WebEngine segfault(139) 仍为 #9 既有现象。

## 后续（再规划）

- 脚本型/动态片段、片段快捷键绑定（绑定归 #17/#20）、模板选择器对话框（替代文本输入模板名）、缩进对齐首行 → 后续小迭代。
