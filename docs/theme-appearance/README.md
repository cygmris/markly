# Markly — Theme & Appearance（theme-appearance spec #2）

主题/外观系统：三套风格（A 精炼经典 / B 沉浸写作 / C 工作台）× 浅/深 × 7 强调色 × 布局（左右栏 + 密度），实时切换 + 本地持久化。供 spec #3 外壳与所有 UI 直接使用。

## 语义 token 角色（`src/core/theme/tokens.h` 的 `ThemeTokens`）

UI **只引用语义角色**，不写死风格颜色，故风格可热切换。

- 表面：`window`(应用底) `rail`(活动栏) `sidebar`(树/列表) `canvas`(编辑器) `card`(卡片) `bar`(标题/状态栏)
- 线/文：`border` `borderStrong` `text` `dim` `faint` `gutter`
- 活动栏文本（C 深色 rail 专用，A/B 回退）：`railText` `railDim` `railActive`
- 强调：`accent` `accentText`(其上文字) `accentSoft`(低透明填充) `selection`(选中) `hover`
- 编辑器语义：`heading` `emphasis` `codeInline` `link` `codeBg` `codeInk`
- 杂项：`windowButton`；风格扩展色 `lime` `amber` `pink` `sage`
- 字体角色：`fontUi`(Hanken Grotesk) `fontSerif`(Newsreader) `fontDisplay`(Space Grotesk) `fontMono`(JetBrains Mono)，均含中文回退
- 度量：`contentZoom`(密度) `radiusSmall/Medium/Large`(6/8/10) `isDark`

## 三套风格映射（`ThemeModel`，逐值复刻设计稿 tok()）

| 设计稿键 | A(refined) | B(focus) | C(bold) |
|---|---|---|---|
| window | win | app | app |
| rail | rail | rail | nav |
| sidebar | sidebar | list | nav |
| canvas | canvas | paper | canvas |
| card | =canvas | =paper | card |
| 默认强调色(浅/深) | `#0e8c6f`/`#2fbf9b` | `#bd6234`/`#e0905c` | `#6c4be0`/`#8b6cf0` |

- 强调色覆盖：传入预设 hex，派生色（selection/accentSoft 等）用 `ColorUtils::alpha` 复刻 `vAlpha(acc, x)`。
- `accentText` 按强调色亮度自动选黑/白（`ThemeModel::accentTextFor`）。
- 入口：`ThemeModel::buildTokens(StyleId, bool dark, QColor accentOverride)`（纯函数，无效 accent → 风格默认）。

## Appearance 偏好与持久化

- 字段：`style`(refined/focus/workbench)、`theme`(light/dark/auto)、`accent`(default/teal/blue/violet/ochre/rose/green)、`show_left`、`show_right`、`density`(compact/normal/roomy)。
- 存储：`WidgetConfig.appearance` 段（`~/.config/Markly/Markly/markly.json` 的 `widget.appearance`）；缺省走 DEFAULTS（Refined/Light/default/true/true/normal）。仅在用户改动时写入。
- `Appearance`（QObject）：每个 setter 即时持久化 + 发 `changed()`；`theme=auto` 跟随 `QStyleHints::colorScheme`；`resetDefaults()`。

## QML 用法（spec #3 外壳直接照此绑定）

主窗口注入两个上下文属性：
```cpp
quick->rootContext()->setContextProperty("Theme", &themeMgr);          // ThemeMgr*
quick->rootContext()->setContextProperty("Appearance", themeMgr.getAppearance());
```
QML 内：
```qml
Rectangle { color: Theme.canvas; border.color: Theme.border }
Text { color: Theme.text; font.family: Theme.fontUi }
// 读写偏好（写即持久化 + 全 UI 刷新）
MouseArea { onClicked: Appearance.style = 2 }   // 切到工作台 C
```
所有 `Theme.*` 为 Q_PROPERTY，统一 `NOTIFY themeChanged` → 改 Appearance 即全绑定刷新（无需重建组件）。

## QWidget 侧 QSS

`ThemeMgr::fetchQtStyleSheet()` 由当前 token 生成基础控件 QSS（`StyleSheetGenerator`）；`themeChanged` 时 MainWindow 重新 `qApp->setStyleSheet(...)`。spec #1 的 Application 主题热重载通路无需改动。

## 校验载体

占位 `MainWindow` 用 `QQuickWidget` 承载 `qrc:/qml/theme/ThemePreview.qml`（token swatch + 「外观」抽屉 `AppearancePanel.qml`），证明实时切换 + 持久化。spec #3 用真实外壳替换该 QML 内容。

## 文件

- `src/core/theme/`：tokens.h、thememodel.{h,cpp}、appearanceconfig.{h,cpp}、appearance.{h,cpp}、stylesheetgenerator.{h,cpp}
- `src/core/thememgr.{h,cpp}`：QML 桥接（替换 spec #1 占位）
- `src/utils/colorutils.{h,cpp}`：alpha/对比度
- `src/qml/theme/`：AppearancePanel.qml、ThemePreview.qml
- 测试：`tests/test_thememodel.cpp`（token 精度 + accent 派生 + 持久化往返）

## 给后续 spec 的注意

- spec #3 外壳的每个面板/控件应只引用 `Theme.*` 角色；新加颜色用途先在 `ThemeTokens` 加角色并在三个 `build*` 填值。
- QML `font.pixelSize` 必须为整数（QML 限制）；分数字号会编译失败。
- 偏好新增字段：扩展 `AppearanceConfig` + `Appearance` Q_PROPERTY + `tokens.h` 预设表。
