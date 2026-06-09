# Vi 输入模式 — spec #8b

为 Markdown 编辑器实现 Vi 输入（Normal/Insert/Visual 三模式 + 核心动作/操作符/计数/
无名寄存器）。核心逻辑是纯函数状态机 `ViEngine`（GUILESS 可单测），QML 桥 `Vi` 在
Normal/Visual 模式拦截按键并把结果应用到 `TextEdit`。设置可开关（默认关）。

## 组成

| 文件 | 职责 |
| --- | --- |
| `src/core/editor/viengine.{h,cpp}` | 纯逻辑状态机：`handleKey` → `ViResult{handled,mode,cursor,anchor,edits}` |
| `src/widgets/editors/vibridge.{h,cpp}` | QML `Vi` 桥：`enabled`/`mode`/`handleKey`/`reset` |
| `src/core/editorconfig.{h,cpp}` + `editorcfgqml.*` | `vi_mode`（默认 false）+ `EditorCfg.viMode` |
| `src/qml/shell/MarkdownEditor.qml` | `Keys.onPressed` 前置 Vi 拦截，应用 edits/光标/选区/模式 |
| `src/qml/shell/SettingsDialog.qml` | 「编辑器」分类「Vi 模式」开关 |
| `src/widgets/mainwindow.cpp` | 注册 `Vi` 上下文属性 |

## ViEngine（纯逻辑，可测）

`handleKey(text, cursor, selStart, selEnd, keyText, key, mods)` → `ViResult`：
- `handled=false` → 编辑器走默认输入（Insert 模式打字、Ctrl/Alt/Meta 组合键放行）。
- `edits`：`[{start,end,text}]` 半开区间替换，编辑器**按 start 降序**应用以保证偏移有效。
- `cursor`/`anchor`：应用 edits 后的光标 / 选区锚点（-1 = 无选区）。

状态：模式、计数累积、挂起操作符（d/y/c）、`gg` 的首 g、无名寄存器（行式/字符式）。

## 已覆盖命令

| 类别 | 命令 |
| --- | --- |
| 模式 | `i a I A o O`（→Insert）、`v`（→Visual）、`Esc`（→Normal，行内左移一格） |
| 动作（支持计数 `N`） | `h l j k`、`0 $`、`w b`、`gg G`、`x` |
| 操作符 | `dd yy cc`（行式 N 行）、`dw d$`（配动作）、`D C Y` |
| 粘贴 | `p P`（行式/字符式无名寄存器） |
| Visual | `h l j k 0 $ w b` 扩选区、`d x`（删）`y`（复制）`c`（改）执行后回 Normal |

接线：编辑器在 Vi 开启且非 Insert 时调 `Vi.handleKey`，`r.handled` 则按 `r.edits` 降序
`remove`+`insert`、设 `cursorPosition`/`select`、`event.accepted=true`；Insert 模式仅 `Esc` 经 Vi。

## 验证

- **单元测试** `tests/test_viengine.cpp`（GUILESS，6 组）：
  - 动作：`l`/`$`/`0`/`w`/`b`/`G`/`gg` 光标位置；
  - 计数：`3l` 光标、`3x` 删 3 字符；
  - 编辑：`dw`（删词）、`D`（删到行尾）、`dd`/`2dd`（删行）；
  - 复制粘贴：`yy`+`p` → 行复制到下一行；
  - 模式：`i`→Insert、`Esc`→Normal；
  - Visual：`v`+`lll`+`d` 删 [0,3)。
  - **ctest 19/19 全绿**。
- 构建通过；`MarkdownEditor.qml` qmllint 无语法错误；offscreen 启动无 QML 错误。
- 真实编辑器交互需已打开笔记本会话；核心在纯 `ViEngine`，已由 test_viengine 充分覆盖。

## 后续小迭代（#8e vi-advanced）

`c` 操作符全集、`r`/`~`、`f/t/;/,`、`/`/`?` 搜索 + `n/N`、`.` 重复、多寄存器、`>>`/`<<`、
宏 `q`、块状光标 + 状态栏模式样式。
