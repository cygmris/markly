# 思维导图视图 — spec #18b

把当前 Markdown 笔记的**标题大纲**渲染成只读交互式思维导图（MindElixir，客户端，离线）。
作为一个 viewMode（edit/read/split/**mindmap**），复用 #6 ViewArea.viewMode + #9 WebEngine。

## 组成

| 文件 | 职责 |
| --- | --- |
| `src/data/web/mindmap.mindelixir.js` | MindElixir（UMD 全局 `MindElixir`，约 80KB；源自 vnote-3201） |
| `src/data/web/mindmap.html` | `renderMindmap(md)`：标题→树→MindElixir 只读渲染 |
| `src/qml/shell/MindmapPane.qml` | WebEngineView 载 mindmap.html，随 `Views.currentText` 重渲染 |
| `src/qml/shell/EditorArea.qml` | mode==="mindmap" 显示 MindmapPane（隐藏编辑器/预览/查看器） |
| `src/widgets/viewarea/viewarea.cpp` | `setViewMode` 放行 `"mindmap"` |
| `src/qml/shell/UnitedEntry.qml` | 命令「查看思维导图」/「编辑模式」 |

## 标题→思维导图

`mindmap.html`：
- `parseHeadings(md)`：逐行匹配 ATX 标题 `^(#{1,6})\s+(.*)$`，跳过围栏代码块 → `{level, text}` 列表。
- `buildTree(headings, rootTitle)`：栈式构造嵌套 `{id, topic, children}`；按 level 维护父链，更深
  level 成为子节点。首个 H1 作根标题（不重复成子节点）；无标题 → 仅根「笔记」。
- `renderMindmap(md)`：`new MindElixir({el, direction: SIDE, editable:false, draggable:false,
  contextMenu:false}).init({nodeData: tree})`。缺 `window.MindElixir` → 纯文本降级。

`MindmapPane` 绑定 `Views.currentText`，载入完成 + 文本变化时 `runJavaScript("renderMindmap(...)")`。

## 验证

- 构建通过；既有 **ctest 23/23 无回归**（纯前端 + QML）。
- **端到端**（throwaway QWebEngineWidgets 加载真实 `mindmap.html`，离线）：
  `renderMindmap("# Root\n## A\n## B\n### A1")`，dump `#map`：
  - MindElixir 真实 DOM：`<div class="map-canvas"><root><tpc>Root</tpc></root>`，子节点
    `<grp class="lhs"><tpc>A</tpc>` / `<grp class="rhs"><tpc>B</tpc>`，`A1` 嵌套在 `B` 的
    `<children>` 下——**层级 = 标题级别**（Root→{A,B}，B→{A1}）；
  - 4 个标题文本 Root/A/B/A1 全部成为节点，0 错误/降级。
  - 证据见 `render-evidence.txt`。
- 注：offscreen WebEngine 不栅格化，以渲染后 DOM 为权威证据（同 #9d/#9e 图表）。

## 后续小迭代（#18c）

思维导图**编辑** + `.mm`/`.json` 文件双向同步、导出 PNG/SVG、主题、节点链接跳转回笔记标题。
