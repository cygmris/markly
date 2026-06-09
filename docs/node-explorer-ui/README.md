# Markly — Node Explorer UI（node-explorer-ui spec #5）

把 spec #4 笔记本数据层接到 spec #3 三套外壳：真实笔记本树替换占位，笔记本/节点交互。**确立「占位→真实 model」桥接范式**，后续 UI spec 照此。

## 桥接范式（重要，后续 UI spec 复用）

```
core 数据(#4 NotebookMgr/Notebook/Node)
  → C++ QObject 桥接(Q_PROPERTY 列表 + Q_INVOKABLE 交互)
  → MainWindow.rootContext setContextProperty 注入 QML
  → QML 绑定属性 + 调 Q_INVOKABLE
```
- 写操作经 #4 `Notebook` 门面（保证磁盘 vx.json + SQLite 三处一致），桥接只读 Node + 写经门面。
- 桥接对象：`NotebookExplorer`(context property `Explorer`) + `DialogHelper`(`Dialogs`，原生对话框)。

## NotebookExplorer（src/widgets/explorer/）

- **Q_PROPERTY**：`visibleNodes`(QVariantList，当前笔记本树展平：nodeId/parentId/name/type/depth/expanded/hasChildren/selected/三色/isExternal)、`notebooks`(id/name/current)、`currentNotebookName`、`hasNotebook`、`selectedNodeId`。
- **Q_INVOKABLE**：toggleExpand/selectNode、newNote/newFolder/renameNode(返回错误串)、removeToRecycle/removePermanent、setNodeColor/clearNodeColor、importExternal、switchNotebook、newNotebookAt/openNotebookAt。
- 监听 `NotebookMgr.currentNotebookChanged/notebookAdded/Removed` → rebuild → emit changed → QML 自动刷新。
- 懒加载：展平时对展开的文件夹调 `Notebook.loadNodeChildren`；`resolveNode(id)` 用 `QHash id→QWeakPointer`。
- 外部文件：`listDir − 受管子节点 − 忽略名(vx_notebook.json/vx.json/vx_notebook.db/vx_images/vx_attachments/vx_recycle_bin/.开头)` → isExternal 项；`importExternal` 调 `Notebook.importNode`（注册已存在磁盘文件，不重写内容）。

## QML

- `MarklyShell.qml`：`Explorer.hasNotebook` 为假 → `EmptyState.qml`（引导新建/打开）；为真 → Loader 加载外壳。
- `ShellRefined.qml`：侧栏树绑 `Explorer.visibleNodes`（TreeRow 接 onClicked 展开/选择、onRightClicked 弹 `NodeContextMenu`、应用节点 nameColor）；侧栏「+」新建；标题栏笔记本 chip 显示 `currentNotebookName` + 点击弹 `NotebookSelector`。
- `ShellFocus.qml`：笔记列表绑可见 file 节点（NoteCard 真实文件名，点击 selectNode）。
- `ShellWorkbench.qml`：墨色导航「笔记本」区绑 `Explorer.notebooks`（点击 switchNotebook）。
- `NodeContextMenu.qml`：新建笔记/文件夹、重命名、设置/清除颜色、删除到回收站、永久删除（confirm）、（外部）导入。
- `NotebookSelector.qml`：列已打开笔记本（切换）+ 新建/打开。
- `EmptyState.qml`：无笔记本引导。

## #4 扩展

`Notebook::importNode(parent,type,name)`：把已存在磁盘文件/夹注册为受管节点（不重写内容；文件夹补 vx.json）。

## 验证

- `tests/test_notebookexplorer.cpp`（ctest 通过）：visibleNodes 展平/展开/折叠、newFolder/newNote/rename/setNodeColor/removeToRecycle、外部文件 isExternal + importExternal 转受管、名称校验。
- 截图（MARKLY_SHOT，手建样例笔记本 /tmp/markly-nb）：
  - `/tmp/markly-shots/explorer-refined.png`：A 真实树（README.md / 算法·读书笔记 文件夹 / draft.md 外部文件弱化）。
  - `explorer-focus.png`：B 笔记列表真实文件。
  - `explorer-workbench.png`：C 导航真实笔记本。
  - 无笔记本 → 空状态。

## 已知 / 后续

- 选中文件发 `MarklyApp.openFileRequested(absolutePath)`，真正打开由 **#6 buffer-view-area / #7 editor** 接。
- 编辑器/预览/大纲仍占位（#7/#9）；B 笔记摘要、C 智能视图(收件箱/全部/最近)与标签区仍占位（#13/#11）。
- 拖拽排序、跨笔记本移动留后续（move 同笔记本已由 #4 支持，UI 拖拽未做）→ 记入 roadmap polish。
