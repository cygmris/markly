# 全局热键 / 单实例呼出 — spec #17b

用户配置一个全局热键（默认 `Ctrl+Alt+M`）把 Markly 主窗口呼到前台。热键字符串解析为纯函数
（可单测）；OS 级全局注册用 X11/XCB（仅 `xcb` 平台，能力守卫）。便携的「呼出」回退已由
**单实例机制**实现（再次启动 = 把已有窗口提到前台）。

## 组成

| 文件 | 职责 |
| --- | --- |
| `src/core/hotkey/hotkeyparser.{h,cpp}` | `parse("Ctrl+Alt+M")` → `{valid, modifiers, key}`（纯函数） |
| `src/widgets/hotkey/globalhotkey.{h,cpp}` | XCB 全局注册（`xcb` 平台守卫）+ 原生事件过滤 → `activated()` |
| `src/widgets/mainwindow.{h,cpp}` | 注册热键 + `activated` → `showMainWindow()` |
| `src/core/widgetconfig.{h,cpp}` | `global_hotkey`（默认 `Ctrl+Alt+M`） |
| `src/qml/shell/SettingsDialog.qml` | 「全局热键」文本字段（TrayCfg.globalHotkey，改后重启生效） |

## HotkeyParser

`QKeySequence(seq, PortableText)` → 单组合键 → `{modifiers, key, valid}`。空串/多组合/非法 → invalid。

## GlobalHotkey（X11/XCB）

- `registerHotkey(seq)`：**`QGuiApplication::platformName() != "xcb"` 时直接 return false**（Wayland/
  offscreen 安全降级，不崩）；否则经 `QNativeInterface::QX11Application::connection()` 取 xcb 连接，
  Qt::Key→keysym→keycode、Qt 修饰位→XCB 掩码，对 root 窗口 `xcb_grab_key`（含 Num/Caps lock 的 4 种
  变体），装 `QAbstractNativeEventFilter`。
- `nativeEventFilter`：`XCB_KEY_PRESS` 且 keycode/mods（去 lock 位）匹配 → `emit activated()`。
- `MainWindow` 把 `activated` 连到 `showMainWindow()`（show+raise+activate）。

## ⚠️ Wayland 限制（诚实记录）

**Wayland 出于安全不允许便携的全局热键**（由合成器掌控）。本机为 Wayland 会话，X11 grab 代码
**编译通过但运行时不触发**（platformName 非 xcb，registerHotkey 直接降级）。

- **跨平台回退**：单实例呼出已实现——第二次 `markly`（或点桌面图标）经 `SingleInstanceGuard.requestShow`
  把已有窗口提到前台。这是便携的「summon」。
- **X11 下验证**：在 X11 会话（`XDG_SESSION_TYPE=x11` 或 `QT_QPA_PLATFORM=xcb`）启动，按配置的热键
  即可呼出窗口。

## 验证

- **单元测试** `tests/test_hotkey.cpp`（GUILESS）：`parse("Ctrl+Alt+M")`（Ctrl+Alt+Key_M）、
  `parse("Meta+Shift+P")`（Meta+Shift+Key_P）、空/空白 → invalid。**ctest 26/26 全绿**。
- 构建通过（markly_core 链接 xcb/xcb-keysyms，GlobalHotkey 编译含 XCB 路径）；SettingsDialog qmllint
  无语法错误；offscreen 启动 GlobalHotkey 安全 no-op、无崩溃。
- X11 grab 运行时不可在本 Wayland 会话验证（诚实记录，X11 会话可手动验证）。
