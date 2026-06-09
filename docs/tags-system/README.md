# Markly — 标签系统（tags-system spec #11）

复用 #4 标签数据层（`tag`/`tag_node` 表、`NotebookTagMgr`、`Notebook::updateNodeTags`）、#12 `queryNodesByTag`、#5 桥接范式、#6 `openFileRequested`、#12 `leftPage` 外壳切页。本 spec 补**标签面板 UI + 节点打标交互 + 标签桥接**。

## 组件

- **NotebookExplorer 打标接口**：`Q_INVOKABLE QString nodeTagsCsv(nodeId)`（当前标签逗号串）、`void setNodeTags(nodeId, csv)`（切分/trim/去空/去重 → `Notebook::updateNodeTags` → `emit MarklyApp::tagsChanged` + `rebuild`）。复用既有 `resolveNode`/`currentNotebook`，不新建重复实体。
- **MarklyApp::tagsChanged()**：标签变更枢纽信号。
- **TagBridge**（`widgets/tags/`，context property `Tags`，放 markly_core）：只读面板数据源。
  - `tags`（`[{name,count}]`，count = `queryNodesByTag(tag).size()`）、`selectedTag`、`taggedNodes`（`[{name,path}]`）。
  - `selectTag/openTaggedNode/refresh`；节点解析用 `collectMarkdownNodes` 映射（同 Searcher）。
  - connect `NotebookMgr::currentNotebookChanged` 与 `MarklyApp::tagsChanged` → `refresh`。
- **TagsPanel.qml**：标签 Chip（`#name` + count，选中 accent）+ 选中标签的笔记 ListView（name + path）+ 空状态。点击标签筛选，点击笔记 `Tags.openTaggedNode` 打开。
- **NodeContextMenu「标签…」**：仅文件节点；`Dialogs.promptText` 预填当前标签 → `Explorer.setNodeTags`。
- **三外壳集成**：A/B 活动栏 `tag` 按钮、C ink-nav「标签」标题 → `leftPage = "tags"`；左 dock 嵌 `TagsPanel`（与 explorer/search 互斥可见）。

## 修复的两个既有 Bug（#4 数据层，被 #11 暴露）

1. **`addTag` 用 `INSERT OR REPLACE`** → 第二次为另一节点添加同名标签时，REPLACE 先删 `tag` 行，经 `tag_node` 的 `FK ON DELETE CASCADE` **连带删掉前一节点的 tag_node 链接**，导致计数丢失。改为 **`INSERT OR IGNORE`**（标签已存在则不动）。
2. **`Notebook::open` 中 `tagMgr->load` 早于 `rebuildDatabase`** → 首开笔记本时 tag 表尚空，`getTags()` 拿不到标签（但直接 `queryNodesByTag` 能查到，故表现为「面板无标签 chip 但选标签有节点」）。修复：rebuild 后**重新 `tagMgr->load`**。

## 验证

- ctest 11/11（新增 `test_tags`：打标→`queryNodesByTag` 计数（算法 2 / DP 1）、`tagMgr.getTags` 含标签、TagBridge `tags` 计数 + `selectTag` 节点数、清空标签 → 计数归零）。
- 截图 `/tmp/markly-shots/tags2.png`：标签页显示 #算法 3（选中）/#DFS 1/#DP 1/#二叉树 1/#图 1，下方列出 算法 标签的 3 篇笔记 + 路径。端到端可用。
- offscreen 退出 WebEngine segfault(139) 仍为 #9 既有现象，截图先存不受影响。

## 已知简化 / 后续（再规划）

- **标签父子层级树 UI**：NotebookTagMgr 已存父子图（tag_graph），本 spec 以扁平 chip 呈现；层级树/折叠 → polish 或后续小迭代。
- **标签管理**（重命名/删除/移动标签）、**拖拽节点到标签打标** → 后续小迭代或并入 #19 设置。
- `promptText` 取消与清空都传空串 = 清除（不区分取消）→ 若要区分需 promptText 返回 null，记 polish。
