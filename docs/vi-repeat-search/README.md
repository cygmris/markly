# Vi `.` 重复上次修改 — spec #8g

为纯逻辑 `ViEngine`（#8b/#8e/#8f）实现 Vim 的 `.`（重复上次修改），通过「修改录制 + 回放」，
全在引擎内（GUILESS 可单测，无新依赖、无 QML 改动）。

## 机制

- **录制**：`handleKey` 的 Normal 分支包一层——把构成当前命令的按键累积到 `m_curKeys`；当命令
  「歇下」（`m_pendingOp`/`m_pendingChar`/`m_gPending`/`m_count` 全空）且**产生了编辑**且**停在
  Normal** 时，提交 `m_lastChange = m_curKeys`，清空 `m_curKeys`。
  - 纯移动/模式切换（`l`/`j`/`i`…）不产生编辑 → 不录为修改。
  - 多键命令（`dd`/`dw`/`rX`/`2x`/`>>`）跨多次 `handleKey`，录制持续累积直到歇下。
- **回放**（`.`）：`m_replaying=true`，把 `m_lastChange` 的按键逐个**用同一原始光标**重入
  `handleKey`，累积各子调用的 `edits`，取末光标/模式返回。
  - 不变量：录制序列里只有最后的编辑键产生 `edits`，setup 键（`d`/`r`/计数）只改引擎状态、不移
    光标，故所有 `edits` 都相对传入的原文有效，可直接交调用方应用。
  - 防重入标记 `m_replaying` 使回放期间不再录制、`.` 不递归。

## 已覆盖 vs 边界

- ✅ 重复停在 Normal 的修改：`x`/`dd`/`dw`/`D`/`r<c>`/`~`/`>>`/`<<`/`p`（含计数与参数键）。
- ⛔ 进入 Insert 的修改（`c`/`s`/`S`/`i`/`o` + 其后键入文本）暂不在 `.` 范围（需录制 Insert 期
  键入）→ #8h。

## 验证

- **单元测试** `tests/test_viengine.cpp`（扩展，GUILESS）：
  - `x` 后 `.`：`"abcdef"`→`"bcdef"`→`"cdef"`；
  - `dd` 后 `.`：`"a\nb\nc"`→`"b\nc"`→`"c"`；
  - `~` 后 `.`：`"abc"`→`"Abc"`(cur1)→`"ABc"`(cur2)；
  - `rX`+`l`+`.`：`"abc"`→`"Xbc"`→（移到 1）→`"XXc"`；
  - `2x` 后 `.`：`"abcdef"`→`"cdef"`→`"ef"`（重复计数）；
  - 无录制 `.`：不变。
  - **ctest 23/23 全绿**（test_viengine 15 组：#8b 6 + #8e 5 + #8f 3 + #8g 1）。
- 构建通过；MarkdownEditor.qml 未改（qmllint 干净）；offscreen 启动无错。

## 后续（#8h vi-macros-search）

`.` 重复 Insert 类修改（录制 Insert 期键入）、`/`/`?` 搜索 + `n`/`N`（编辑器查找框集成）、
宏 `q`/`@`、可视块 `Ctrl-V`。
