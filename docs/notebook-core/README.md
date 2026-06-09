# Markly — Notebook Core（notebook-core spec #4）

笔记本数据层（纯 core，无 UI）：磁盘格式与 SQLite schema **兼容 VNote v3.x**，可直接打开存量笔记本。替换了 spec #1 `MarklyApp::getNotebookMgr()` 占位（现返回真实 NotebookMgr）。

## 磁盘格式（VNote 兼容）

- **`vx_notebook.json`**（笔记本根）：`version`(3) / `name` / `description` / `image_folder`(vx_images) / `attachment_folder`(vx_attachments) / `created_time`(ISO8601 UTC) / `version_controller`(dummy.vnotex) / `config_mgr`(vx.vnotex) / `history`(原样保留) / `tag_graph`("parent>child;") / `extra_configs`。
- **每目录 `vx.json`**：顶层 `version/id/signature/created_time/modified_time/background_color/border_color/name_color/files[]/folders[]`。`files[]` 项含 `name/id/signature/created_time/modified_time/attachment_folder/tags[]/三色`；`folders[]` 项含 `name/三色`（子文件夹自身的 id/时间在它自己的 vx.json 顶层）。
- 回收站：`vx_recycle_bin/yyyyMMdd/`。
- SQLite 索引：`vx_notebook.db`（根目录）。

## SQLite schema（VNote 兼容）

- `node(id INTEGER PK, name TEXT NOT NULL, signature INTEGER NOT NULL, parent_id INTEGER → node.id ON DELETE CASCADE)`
- `tag(name TEXT PK, parent_name TEXT → tag.name ON DELETE CASCADE) WITHOUT ROWID`
- `tag_node(node_id → node.id, tag_name → tag.name, 双向 CASCADE)`，`PRAGMA foreign_keys=ON`。

**真源 vs 索引**：`vx.json` 是真源，SQLite 是可查询索引。首次打开（含 VNote 笔记本，Markly 的 db 不存在时）从 vx.json 全量重建索引；之后 CRUD 双写。node.id 由 db autoincrement 生成后回写 vx.json。

## 类（已做架构精简，见下）

- `INotebookBackend` / `LocalNotebookBackend`：文件系统后端（相对根路径）。
- `vx_node_config::{NodeConfig,NodeFileConfig,NodeFolderConfig}`：vx.json 序列化。
- `NotebookConfig`：vx_notebook.json 序列化。
- `Node` + `NodeVisual`：节点树（文件夹/文件，懒加载，三色视觉，路径计算）。
- `NotebookDatabaseAccess`：SQLite 三表 CRUD（含 `insertNode` 返回生成 id）。
- `NotebookTagMgr`：标签树（DB tag 表 + tag_graph 解析/序列化）。
- `IVersionController` / `DummyVersionController`：版本控制占位（no-op）。
- `Notebook`：CRUD 门面（聚合 backend/db/tagmgr/vc），三处双写。
- `NotebookMgr`：多笔记本注册表 + 当前笔记本 + 持久化（SessionConfig 的 notebook_root_paths / current_notebook_root_path），接入 MarklyApp。

### 架构精简（相对 VNote 源 + 已审批 design 的偏差，已评估）

按「避免重复实体」原则，单实现的接口已合并，**不影响磁盘/DB 兼容与验收**：
- `Node`/`VXNode` 合并为单一 `Node`（仅一种 vx 格式 + local 后端，路径逻辑内联）。
- `Notebook`/`BundleNotebook` + `INotebookConfigMgr`/`VXNotebookConfigMgr`/`BundleNotebookConfigMgr` 合并为单一 `Notebook`（vx.json 加载/CRUD 内聚其中）。
- 工厂（NotebookFactory/ConfigMgrFactory/...）折叠进 `NotebookMgr` 直接装配。

### 延后项（已重新规划，见 roadmap）

- **`ExternalNode`（外部文件：磁盘存在但 vx.json 未追踪）** → 延后到 **#5 node-explorer-ui**（与「扫描笔记本/导入外部文件」UI 一起做更自然）。已在 roadmap 记录。

## 节点 CRUD 三处双写顺序（关键）

- **新建笔记**：`db.insertNode(name,sig,parentId)` 得 id → `backend.writeFile`（空文件）→ 内存 addChild → 写父目录 vx.json。
- **新建文件夹**：额外先 `mkdir` + 写子目录自身空 vx.json，再写父目录 vx.json。
- **重命名**：backend rename → 更新内存 + db.updateNode → 重写父 vx.json。
- **永久删除**：backend remove（递归）→ db.removeNode（CASCADE 清子节点 + tag_node）→ 移出内存 + 重写父 vx.json。
- **回收站**：move 到 `vx_recycle_bin/yyyyMMdd/` → db.removeNode → 移出树。
- **颜色**：文件节点写父 vx.json 的 files[] 项；文件夹节点写本目录 vx.json 顶层（+ 父 folders[] 项）。
- **标签**：db tag_node + NotebookTagMgr + 同步 tag_graph 到 vx_notebook.json。

## 接入与后续 spec

- `MarklyApp::getNotebookMgr()` 返回真实实例；`initLoad()` 调 `loadNotebooks()` 恢复上次打开的笔记本。
- **#5 node-explorer-ui** 用 NotebookMgr/Node 真实 model 替换 shell `DemoData.TREE` 占位 + 实现 ExternalNode/导入。
- 标签面板 **#11**、全文搜索 **#12** 复用 NotebookDatabaseAccess / NotebookTagMgr。
- 编辑器 **#7** 用 Node 的内容文件路径打开/保存。

## 测试

`tests/test_notebookcore.cpp`（5 例，ctest 通过）：创建笔记本格式；创建文件夹/笔记 **磁盘+vx.json+SQLite 三处一致**；重命名/颜色/回收站；关闭后重开懒加载一致；**手写 VNote v3 样例 openNotebook 正确读出节点树 + 重建 DB 索引**。

## 构建

`find_package(Qt6 ... Sql)`；notebook-core 源在 markly_core，链接 Qt6::Sql。
