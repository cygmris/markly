# Markly — App Shell UI（app-shell-ui spec #3）

主窗口外壳：无边框自绘窗口 + QML 外壳，三套风格（A/B/C）由 spec #2 的 `Theme`/`Appearance` 单例驱动实时切换。已替换 spec #1/#2 占位内容（ThemePreview → MarklyShell）。

## 结构

```
MainWindow : FramelessMainWindow (C++)
  └─ QQuickWidget → qrc:/qml/MarklyShell.qml
       ├─ Loader (按 Appearance.style: 0=A / 1=B / 2=C)
       │    ├─ shell/ShellRefined.qml    (A 三栏 IDE)
       │    ├─ shell/ShellFocus.qml      (B 沉浸写作)
       │    └─ shell/ShellWorkbench.qml  (C 工作台)
       ├─ 「外观」悬浮按钮 + theme/AppearancePanel.qml 抽屉(复用 spec #2)
       └─ 注入上下文属性: Theme / Appearance / Win
```

- **三套风格 = 三套 QML 布局 + 共享子组件**（不是三份重复 UI）：结构差异在 Shell*.qml，配色全部引用 `Theme.*` 角色 → 切主题/强调色自动适配。
- **布局开关**：`Appearance.showLeft/showRight` 显隐侧栏；`Theme.contentZoom`（密度）缩放内容区。

## 共享子组件（src/qml/shell/components/）

WinControls（窗口控制→Win.*）、RailButton（活动栏）、StatusBar、Chip、EditorTab、EditorToolbar、TreeRow、OutlineRow、NoteCard（B）、Omnibar（C）。全部只引用 `Theme.*`。

## 图标系统（src/qml/icons/）

- `IconPaths.js`：约 50 个单色线性图标的 SVG 路径（24x24，复刻 icons.jsx）。
- `Icon.qml`：用 `QtQuick.Shapes` 的 Shape/ShapePath/PathSvg 渲染描边（多子路径 join 成一条 PathSvg），按 `size/24` 缩放，`color` 默认绑 Theme 角色。
- 用法：`Icon { name: "search"; size: 17; color: Theme.dim }`。

## 无边框窗口（src/widgets/framelessmainwindow/）

`FramelessMainWindow`（QMainWindow 子类，Qt::FramelessWindowHint）：
- `Q_INVOKABLE` 暴露给 QML（经 `Win`）：`startMove()`（标题栏拖动 → startSystemMove）、`startResize(int edges)`（边缘 → startSystemResize）、`toggleMaximize()`/`minimizeWindow()`/`closeWindow()`/`isMaximizedWindow()`。
- QML 标题栏 MouseArea：`onPressed: Win.startMove(); onDoubleClicked: Win.toggleMaximize()`。
- 几何持久化复用 spec #1 SessionConfig（closeEvent 存、构造恢复）。

## 占位数据（src/qml/shell/DemoData.js）

TREE/TABS/OUTLINE/TAGS/NOTES/NAV_ITEMS/NOTEBOOKS/NAV_TAGS/PROPS/BACKLINKS/EDITOR_LINES。**后续 spec 用真实 model 替换同名结构**：笔记本树→#5、编辑器→#7、大纲→#9、搜索/标签→#11/#12。

## 构建与运行

```bash
cmake --build build
QT_QPA_PLATFORM=offscreen ./build/src/markly        # 无显示环境
./build/src/markly                                  # 真实窗口
```

## 截图验证（dev 工具）

`MARKLY_SHOT=/path.png` 环境变量：启动后 1.2s 抓 QQuickWidget framebuffer 存 PNG 并退出（offscreen 可用）。本 spec 验证截图：
- `/tmp/markly-shots/{refined,focus,workbench}-light.png`、`refined-dark-blue.png`
- 三套风格 + 浅/深 + 强调色覆盖均渲染贴近设计稿。

## 接线点 / 已知项（后续 spec）

- 标题栏「新建/导入/快速记录」当前为占位（任务要求连 MarklyApp 信号，留作 spec #4/#5 接线）。
- 状态栏右侧项暂未右对齐（QML Row 无 flex）；polish 留 spec #20。
- 截图中偶见极小 "、" 像素（offscreen framebuffer 抓取伪影，非应用内容，源码无此字符）。
- 真实数据接入后，Shell*.qml 的 Repeater model 从 DemoData 换成 C++ 暴露的 model。
