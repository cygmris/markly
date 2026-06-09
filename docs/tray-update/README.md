# 系统托盘 + 更新检查 — spec #20b

实现系统托盘（QSystemTrayIcon）+ 可选最小化到托盘，以及从 GitHub Releases 检查新版本。
版本解析/比较是纯函数（可单测）；托盘需桌面会话，离屏以 `isSystemTrayAvailable()` 守卫降级。

## 组成

| 文件 | 职责 |
| --- | --- |
| `src/core/update/updatechecker.{h,cpp}` | `parseLatestVersion`/`isNewer`（纯函数）+ 异步 `check` |
| `src/widgets/update/updatebridge.{h,cpp}` | QML `Update`：`check()` + `checked` 信号 |
| `src/widgets/update/traycfg.{h,cpp}` | QML `TrayCfg`：`minimizeToTray` 属性 |
| `src/core/widgetconfig.{h,cpp}` | `minimize_to_tray`（默认 false） |
| `src/widgets/mainwindow.{h,cpp}` | `setupTray()`（守卫）+ `closeEvent` 最小化到托盘 |
| `src/qml/shell/UnitedEntry.qml` | 命令「检查更新」+ 结果通知 |
| `src/qml/shell/SettingsDialog.qml` | 「外观」分类「最小化到托盘」开关 |

## 更新检查

- `UpdateChecker::check(owner, repo, current)`：GET
  `https://api.github.com/repos/<owner>/<repo>/releases/latest`，解析 `tag_name`（去前导 `v`），
  用 `QVersionNumber` **数值比较**（`1.10.0` > `1.9.0`，非字典序），`emit result(hasUpdate, latest, error)`。
- `UpdateBridge.check()` 用占位 owner/repo + `qApp.applicationVersion()`。**发布前需把
  `updatebridge.cpp` 的 `c_owner`/`c_repo` 改成真实 release 仓库。**
- 统一入口「检查更新」→ `Update.check()`，`onChecked` 通知「有新版本 X / 已是最新 / 检查失败」。

## 系统托盘

- `setupTray()`：`if (!QSystemTrayIcon::isSystemTrayAvailable()) return;`（离屏/无托盘 DE 安全降级，
  **smoke_startup 不受影响**）。托盘图标 `:/markly.png`（#21 打包图标，QRC alias）+ 菜单
  （显示 Markly / 退出）；点击托盘激活主窗口。
- `closeEvent`：`minimize_to_tray` 开启且托盘可见 → `hide(); ignore()`（隐藏到托盘而非退出）。
- 设置「最小化到托盘」（TrayCfg.minimizeToTray，默认 false）持久化到 `widget.minimize_to_tray`。

## 验证

- **单元测试** `tests/test_update.cpp`（GUILESS，不打真实网络）：
  - `parseLatestVersion("v1.2.0")` → `"1.2.0"`、无 v 原样、`{}`/非 JSON → `""`；
  - `isNewer`：`1.2.0>1.0.0` 真、相等假、更低假、`1.10.0>1.9.0` 真（数值）、空 latest 假。
  - **ctest 23/23 全绿**（含 2 集成 smoke；托盘守卫使 smoke_startup 不受影响）。
- 构建通过；UnitedEntry/SettingsDialog qmllint 无语法错误；offscreen 启动无错。
- 真实更新检查需网络（离线不测）；托盘需桌面会话（离屏守卫降级）；版本逻辑单测覆盖。

## 后续小迭代（#20c）

文件关联（.md MIME 注册）、NavigationMode（Vim 式跨面板导航键）、托盘通知偏好、自动检查
更新周期、托盘最小化首次提示气泡。
