# 桌面集成 — spec #20c

补全主窗口/桌面集成的剩余项：**自动检查更新**（按周期）、**文件关联**（.md → Markly）、
**跨面板导航命令**（NavigationMode 的实用落地）。这是桌面集成的收口 spec，剩余仅是安装文档。

## 组成

| 文件 | 职责 |
| --- | --- |
| `src/core/update/updatechecker.{h,cpp}` | `shouldCheck`（周期判定，纯函数） |
| `src/core/widgetconfig.{h,cpp}` | `auto_update_check` + `last_update_check`（ms） |
| `src/widgets/update/updatebridge.{h,cpp}` | `autoCheckIfDue()`（到期才检查并更新时间戳） |
| `src/core/desktop/fileassoc.{h,cpp}` | `registerCommands`（xdg 命令，纯）+ `registerNow`（最佳努力） |
| `src/qml/MarklyShell.qml` | 启动 `Component.onCompleted` → `Update.autoCheckIfDue()` |
| `src/qml/shell/UnitedEntry.qml` | 命令「聚焦：编辑器」 |
| `src/qml/shell/SettingsDialog.qml` | 「自动检查更新」开关 |

## 自动检查更新

- `UpdateChecker::shouldCheck(lastMs, nowMs, intervalDays)`：`last==0`（从未）→ true；`last<0` →
  false；否则 `now - last >= intervalDays*86400000`。
- `auto_update_check`（默认 false）+ `last_update_check`（ms）持久化（TrayCfg.autoUpdateCheck 读写）。
- 启动时 `Update.autoCheckIfDue()`：若开启且 7 天周期到 → `setLastUpdateCheck(now)` + `check()`，
  结果由 #20b 通知链提示（仅有更新时打扰）。

## 文件关联

`FileAssoc::registerCommands()` → `["xdg-mime", "default", "markly.desktop", "text/markdown"]`。
手动关联（安装后一次）：

```bash
xdg-mime default markly.desktop text/markdown
update-desktop-database ~/.local/share/applications
```

`registerNow()` 在 xdg-mime 可用时最佳努力执行（headless/无 xdg 时返回 false）。

## 导航命令

UnitedEntry 已有「聚焦：搜索/标签/快速访问/片段」（#17）；本 spec 补「聚焦：编辑器」
（`Views.setViewMode("edit")`），合起来即 NavigationMode 的实用落地——键盘命令在面板间切换焦点。

## 验证

- **单元测试** `tests/test_update.cpp`（扩展，GUILESS）：
  - `shouldCheck`：从未(0)→真、8 天前→真、1 天前→假、`-1`→假；
  - `FileAssoc::registerCommands` 含 `xdg-mime`/`markly.desktop`/`text/markdown`。
  - **ctest 25/25 全绿**（test_update 4 用例）。
- 构建通过；MarklyShell/UnitedEntry/SettingsDialog qmllint 无语法错误；offscreen 启动无错。
- 自动检查的网络、文件关联的 xdg 执行不在测试跑（能力守卫 + 文档）。
