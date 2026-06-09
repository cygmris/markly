# Vi WORD / 替换 / 配对 — spec #8f

继续扩展纯逻辑 `ViEngine`（#8b/#8e，GUILESS 可单测，无新依赖、无 QML 改动）。

## 新增命令

| 类别 | 命令 |
| --- | --- |
| WORD 动作（计数） | `W`（下个 WORD 起点）、`B`（上个 WORD 起点）、`E`（WORD 末尾） |
| 替换 | `s`（删光标处字符进 Insert，`Ns` 删 N 个）、`S`（清当前行内容进 Insert） |
| 配对跳转 | `%`（跳到本行第一个 `()[]{}` 的匹配括号，计嵌套） |

**WORD vs word**：WORD（大写动作）只按**空白**分隔，故 `"a.b"` 整体是一个 WORD；而 #8e 的
`w/e/b`（小写）把标点单独成词。

## 实现

- `WORDForward/WORDBackward/WORDEnd`：仅看 `QChar::isSpace()` 划分 WORD。
- `matchBracket`：从光标到行尾找第一个括号，开括号向后扫、闭括号向前扫，深度计数找匹配；
  无括号/无匹配则光标不动。
- `s`：删 `[cursor, min(cursor+count, lineEnd))` + 进 Insert；`S`：删 `[lineStart, lineEnd)` 内容
  （保留空行）+ 进 Insert（register 行式）。

## 验证

- **单元测试** `tests/test_viengine.cpp`（扩展，GUILESS）：
  - WORD：`"a.b cd"` → `W`=4、`E`=2、从 4 `B`=0；
  - `s`：`"abc"` `s`→`"bc"` + Insert；`S`：`"ab\ncd"` `S`→`"\ncd"` + Insert；
  - `%`：`"(ab)"` 0↔3、`"a(b)c"` 0→3。
  - **ctest 23/23 全绿**（test_viengine 14 组：#8b 6 + #8e 5 + #8f 3）。
- 构建通过；MarkdownEditor.qml 未改（qmllint 干净）；offscreen 启动无错。

## 后续（#8g vi-repeat-search）

`.`（重复上次修改，需修改录制：记录命令 kind/count/arg 并 replay）、`/`/`?` 搜索 + `n`/`N`
（与编辑器查找框集成）、宏 `q`/`@`、可视块 `Ctrl-V`。
