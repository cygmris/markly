# Markly — 快速访问 / 历史 / 闪念（quickaccess-history-flash spec #13）

实现 最近历史（History）+ 快速访问（QuickAccess 固定）+ 闪念（Flash 速记）。复用 #6 `openFileRequested`、SessionConfig 持久化、#12 `leftPage`、#5 桥接范式。

## 组件

- **SessionConfig**：新增 `history`（MRU 路径数组，最近在前）、`quick_access`（固定路径数组）持久化。
- **HistoryMgr**（`core/historymgr`，markly_core）：
  - `addToHistory(path)`：MRU 去重 + 上限 50 + persist + emit changed。
  - `pin/unpin/isPinned/quickAccess`：固定列表去重 + persist。
  - `ensureFlashFile()`：`<AppConfigLocation>/flash.md`，不存在则写模板「# 闪念」。
  - `load()`：从 SessionConfig 恢复。
  - **接线**：`MarklyApp` 持有 `HistoryMgr`，`connect(openFileRequested → addToHistory)` —— 所有打开来源（资源管理器/搜索/标签/历史/闪念）自动进历史；`initLoad` 调 `load`。
- **QuickBridge**（`widgets/quick`，context property `Quick`，markly_core）：`Q_PROPERTY history/quickAccess`（`[{name,path}]`，history 过滤不存在文件）；`Q_INVOKABLE open/pin/unpin/isPinned/openFlash`。
- **QuickAccessPanel.qml**：「已固定」区（点击打开 / × 取消固定）+「最近」区（点击打开），均空状态提示。内联 `NoteRow` 组件。
- **NodeContextMenu「固定到快速访问 / 取消固定」**：文本随 `Quick.isPinned(Explorer.nodeAbsPath(id))` 切换。
- **三外壳**：A/B history 活动栏按钮、C ink-nav「最近」项 → `leftPage="quick"`；左 dock 嵌 `QuickAccessPanel`。A 顶部「快速记录」→ `Quick.openFlash()`。

## 验证

- ctest 13/13（新增 `test_history`：MRU 顺序、去重上移、cap 50 截断、pin/unpin/isPinned、persist/load 往返、ensureFlashFile 创建）。
- 截图 `/tmp/markly-shots/quick13.png`：快速访问页显示「已固定」二叉树.md（带 × ）+「最近」动态规划/图论/二叉树（MRU），history 按钮高亮。
- offscreen 退出 WebEngine segfault(139) 仍为 #9 既有现象。

## 后续（再规划）

- QuickAccess 支持文件夹、历史时间戳排序/显示、Flash 多页/按日归档 → 后续小迭代。
- 历史项右键「从历史移除」、清空历史 → polish。
