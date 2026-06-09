# Markly — Buffer & View Area（buffer-view-area spec #6）

打开/视图层：把笔记节点打开成标签页视图。接住 #5 `selectNode` 发出的 `MarklyApp.openFileRequested(path)`，在视图区以标签打开、编辑、保存、分屏、会话恢复。**ViewWindow 内容为纯文本占位**（#7 用 QWidget 编辑器内核替换）。

## 链路

```
#5 selectNode(file) → MarklyApp.openFileRequested(absPath)
  → MainWindow connect → ViewArea.openFile(path)
  → BufferMgr.open(path)(按路径去重, load 内容)
  → ViewArea 活动 split 加标签 + 设为当前
  → splits 属性变 → EditorArea.qml 显示标签 + 文本
```

## 组件

- **Buffer**（core/buffer/）：一个打开文件的 id/path/name/content/dirty/node。setContent 置 dirty，load/save 读写盘(UTF-8)。
- **BufferMgr**（core/buffer/）：按绝对路径去重 open/get/close；MarklyApp.getBufferMgr 返回(替换 #1 nullptr)。
- **ViewArea**（widgets/viewarea/，context property `Views`）：`QVector<Split>`(每 Split: tabs + active) + activeSplit；`splits`(QVariantList: index/active/currentBufferId/currentName/currentText/tabs[]) + splitCount + hasOpenFile；Q_INVOKABLE openFile/activateTab/closeTab/saveTab/updateText/splitView/unsplit/setActiveSplit；会话持久化(SessionConfig opened_files/current_file)+restoreSession。
- **EditorArea.qml**（共享）：渲染 Views.splits，每 split = 标签栏(点击 activateTab、关闭 closeTab) + 可编辑 TextArea + 保存按钮；空状态"选择一篇笔记开始"；字体按 Appearance.style(A mono/B serif/C ui)。三套外壳编辑区都嵌入它。

## 防绑定循环（关键）

EditorArea 的 TextArea **不绑定** currentText；用 `property cbid: split.currentBufferId; onCbidChanged: editor.text = split.currentText` 仅在切换 buffer 时设文本。`onTextChanged` 调 `Views.updateText`，Buffer.setContent 对相同内容 no-op，故无循环、无误标 dirty。delegate 用 `model: Views.splitCount`(int 稳定) 避免列表变更重建 TextArea 丢光标。

## 会话恢复

SessionConfig 加 `opened_files`/`current_file`。ViewArea open/close/activate 时持久化；启动 `kickOffOnStart` 调 `restoreSession` 重开上次标签(文件存在者)。

## 验证

- `tests/test_bufferview.cpp`（ctest 通过）：BufferMgr 去重/dirty/save、ViewArea 标签/编辑/保存/分屏/激活、会话 opened_files 持久化。
- 截图 `/tmp/markly-shots/editor-refined.png`：预开两文件 → A 显示两标签(README.md + 二叉树…) + 真实文件内容(可编辑) + 侧栏树。三套外壳编辑区均接真实内容。

## 已知 / 后续

- ViewWindow 为纯文本占位 → **#7 editor-core** 用 QWidget 编辑器内核(语法高亮/行号/Vi 等，经 window container 嵌入 QML)替换 EditorArea 的 TextArea。
- 分屏为基础 1-2 split；多分屏/工作区多对多留后续。
- 工具栏格式按钮(EditorToolbar)仍占位 → #8 接编辑命令。
- A 的带行号源码视图被 EditorArea 纯文本取代(占位期)；#7 恢复行号+语法高亮。
