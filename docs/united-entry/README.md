# Markly — 统一入口（united-entry spec #17）

Ctrl+P 命令面板：输入即可**运行命令**或**跳转到笔记**（按名称搜索）。纯 QML，复用 #12 Search、#13 Quick、#2 Appearance、#15 ExportView、#19 SettingsDialog 等既有桥接，无新 C++。

## 再规划：OS 全局热键拆 #17b

vnote 的「全局热键」（桌面任意处呼出应用）需 QHotkey + 平台集成，**Wayland 下受合成器限制不可移植**，拆出 **#17b global-hotkey**（QHotkey X11 可用 + 单实例呼出到前台）。本 spec 只做应用内统一入口（可靠可验证）。

## 组件

- **UnitedEntry.qml**：覆盖层（半透明底 + 顶部居中卡片）。`show()/hide()`；搜索框（`onTextChanged` 非空 → `Search.search(text, AllNotebooks, ObjName)`）。
  - **命令模型**（JS `{label, run}`）：新建笔记、快速记录（闪念）、打开设置、切换深/浅色、聚焦（搜索/标签/快速访问/片段）、导出 PDF/HTML/Markdown。
  - **结果列表**：命令（label 过滤）+ 笔记（`Search.results`）混排，每行带「命令/笔记」类型徽章；`currentIndex` 上下键移动、回车执行、点击执行；执行后关闭。
  - 命令 action：简单的直接调桥接（`Quick.openFlash`/`Appearance.theme`），需外壳能力的经注入的 `actions`（openSettings/setPage/newNote）；导出命令**内嵌 ExportView 自包含**（复用 #15 `doExport` 逻辑）。
- **三外壳集成**：各挂一份 `UnitedEntry`（actions 注入 settingsDialog/leftPage/newRootNote）+ `function openEntry()` + `Shortcut{Ctrl+P}`；工作台 Omnibar 点击 → `unitedEntry.show()`（Omnibar 占位「跳转、搜索或运行命令…」终成真）。
- **MarklyShell.showEntry** + `MARKLY_SHOT_ENTRY` env 截图钩子。

## 验证

- ctest 16/16（不回归，纯 QML 无新 C++）。
- 截图 `/tmp/markly-shots/entry17.png`：统一入口打开，命令列表（新建笔记/快速记录/打开设置/切换深浅/聚焦×4/导出×3）+ 类型徽章 + 首项高亮。输入关键字后追加名称匹配笔记。
- offscreen 退出 segfault(139) 仍为 #9 既有现象。

## 后续（再规划）

- **#17b global-hotkey**：QHotkey 全局热键（X11）、Wayland 限制说明、单实例呼出到前台。
- 更多命令（切换笔记本、最近文件直达）、命令图标、模糊匹配排序 → 待办。
