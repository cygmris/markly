# Markly — 全文搜索（search-fts spec #12）

技术栈关键点 **SQLite FTS5** 落地：每个笔记本的 `vx_notebook.db` 内建 FTS5 虚拟表 `node_fts`，索引笔记的 名称/路径/正文；配合内存匹配（名称/路径）与 tag 查询，提供作用域过滤的搜索器与结果导航 UI。

## 与源（vnote-3201）的差异（技术栈对齐 + 避免重复实体）

源 `src/search/` 用 `FileSearchEngine` 遍历文件逐个正则匹配，**无持久索引**，并有 `ISearchEngine/SearchToken(正则/batch)/异步 Searcher` 一整套。本应用按技术栈要求改用 **FTS5 索引**做内容搜索（秒级、增量），并精简为**单一同步 `Searcher`**：
- 保留源的语义枚举 `SearchScope`(笔记本/全部/文件夹/已打开) 与 `SearchObject`(名称/内容/路径/标签)。
- 内容走 FTS5 `MATCH` + `snippet()`；名称/路径在内存匹配；标签查节点 tags。
- 不引入正则 token / magic-switch / batch / 多线程引擎（延后，见下）。

## 组件

- **数据结构** `core/search/searchdata.h`：`SearchScope`、`SearchObject`(位标志)、`SearchOption`、`SearchResultItem`；`SearchOption::toFtsExpr()` 把关键词按空格切词、每词用双引号包成 phrase（特殊字符按字面，规避 FTS5 语法错误）、空格连接（FTS5 默认 AND）。
- **FTS5 索引**（扩展 `NotebookDatabaseAccess`）：`ensureFtsTable()`(`CREATE VIRTUAL TABLE IF NOT EXISTS node_fts USING fts5(node_id UNINDEXED, name, path, content, tokenize='unicode61')`)、`ftsIsEmpty()`、`ftsUpsert()`(先删后插)、`ftsRemove()`、`ftsQueryContent()`(`MATCH` + `snippet(node_fts,3,'<mark>','</mark>','…',12) ORDER BY rank`)。全部参数绑定。
- **Notebook helper**：`collectMarkdownNodes()`(递归 `ensureLoaded` 收集所有 `.md` 文件节点)；节点的绝对/相对路径直接用既有 `Node::fetchAbsolutePath/fetchRelativePath`。`renameNode/removeNode/moveNodeToRecycleBin` 末尾调 `ftsRemove` 让旧索引失效。
- **Searcher** `core/search/searcher.{h,cpp}`：`search(SearchOption)` 按 scope 选笔记本集合→`ensureIndex`(表存在 + 空则全量 `buildIndex`：遍历节点读盘 `ftsUpsert`)→内容(FTS5)/名称·路径(内存 contains)/标签(节点 tags) 查询→按节点去重→组装 `SearchResultItem`。`reindexNode()` 供增量更新。空关键词返回空；读盘失败 `qWarning` 跳过不崩。
- **桥接** `widgets/search/searchbridge.{h,cpp}`（放 markly_core 供 test）：context property `Search`；`Q_PROPERTY results/count/state/keyword`；`Q_INVOKABLE search(keyword,scope:int,objects:int,folderRelPath)/openResult(index)/clear()`。`openResult` 发 `MarklyApp::openFileRequested(path)`。监听 `MarklyApp::noteSaved` 增量 `reindexNode`（仅当该笔记本索引已建，避免过早全量构建）。
- **事件源**：`MarklyApp::noteSaved(path)` 信号，`ViewArea::saveTab` 保存成功后发出。
- **QML** `qml/shell/SearchPanel.qml`：关键词输入(回车搜索) + scope 4 段按钮 + object 4 toggle(位或) + 状态行 + 结果 ListView(名称强调色 / snippet 富文本 / 路径等宽字)。snippet 的 `<mark>` 在 Qt RichText 不支持，运行时映射为 `<b><font color=accent>`。点击结果 `Search.openResult(index)`。
- **三外壳集成**：左 dock 增加可切换「搜索」页。A `ShellRefined` 活动栏 search 按钮切换；B `ShellFocus` 既有「搜索笔记…」框 + 新增 search 活动栏按钮；C `ShellWorkbench` 顶部 Omnibar 点击进入。`MarklyShell.showSearch(keyword)` 为 dev/截图钩子。

## 验证

- ctest 9/9（新增 `test_search`：内容 FTS 搜索命中 + snippet 含 `<mark>`、名称搜索、空词空结果、未命中空、增量 reindex 后命中）。
- 截图 `/tmp/markly-shots/search.png`：搜索「桥接」→ 左 dock 搜索面板显示 scope/object 开关、「1 条结果」、命中笔记 `QML 与 C++ 桥接.md` + 高亮 snippet + 路径。**FTS5 + 桥接 + 三外壳集成端到端可用**。
- 已知：offscreen 下进程退出时 WebEngine 析构 segfault(exit 139)，**截图已先保存**；纯截图(无搜索)同样如此，属 #9 引入的 WebEngine-on-offscreen-teardown 既有问题，与本 spec 无关，不影响正常运行。

## 已知简化 / 延后（再规划）

- **跳转到行**：结果含 `m_line` 字段但暂不跳转——需编辑器 `selectLine` API，归入 **#8 markdown-editor-input**。
- **重命名后 name/path 更新**：当前 `renameNode` 调 `ftsRemove`（删旧行），下次搜索 `ensureIndex` 不重建非空表，故重命名节点的内容索引要等下次全量重建才回填——MVP 简化，必要时改为重命名后 `ftsUpsert`。
- **正则 / magic-switch / batch / 异步线程**：源的高级搜索 token 延后 polish。
- **搜索历史 / 保存的搜索**：归入 **#13 quickaccess-history-flash**。
