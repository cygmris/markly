# mindmap-edit (#18c) — 可编辑思维导图 + 往返回写

在 #18b（只读思维导图视图）基础上加 **可编辑导图** 与 **导图树 → Markdown 标题往返回写笔记**。
思维导图族至此收口。

## 能力

| 能力 | 实现 |
|------|------|
| 可编辑导图 | `renderMindmap(md, editable)`：`editable=true` 时 MindElixir 以 `editable/draggable/contextMenu/nodeMenu` 全开渲染（增删/改节点、拖拽）。默认无第二参=只读（保持 #18b 兼容）。 |
| 导图树 → Markdown | `treeToMarkdown(node, depth, out)`：根 `#`，每深一层多一个 `#`（深度 d → `d+1` 个 `#`）。`window.mindmapMarkdown()` 读 `mind.getData().nodeData`（offscreen/无 getData 时回退 `lastData`），返回 `out.join("\n\n")`。 |
| 回写笔记 | `MindmapPane.saveToNote()`：`runJavaScript("mindmapMarkdown()")` → 非空且 `bufferId>=0` 时 `Views.updateText(bufferId, md)`。 |
| 触发方式 | ① **Ctrl+S**（仅 editable + 该 pane focus 时）；② 命令面板「保存思维导图到笔记」→ `Views.requestSaveMindmap()` → 信号 `saveMindmapRequested` → 可见的 MindmapPane 响应。 |

## 往返为可验证核心

`md → renderMindmap → mindmapMarkdown` 应得等价标题结构。经 throwaway QWebEngineWidgets
harness（offscreen，加载真实 `src/data/web/mindmap.html`）端到端验证：

```
renderMindmap("# Root\n## A\n## B\n### A1", true)  →  mindmapMarkdown()
----- mindmapMarkdown() -----
# Root

## A

## B

### A1
-----------------------------
ROUNDTRIP PASS
```

harness 源码与输出留在会话 `/tmp/mm_roundtrip/`（不入库根目录，遵全局规范）。

## 命令路由（为何走 Views 信号而非直调 EditorArea）

命令在 `UnitedEntry`，目标 `MindmapPane` 在 `EditorArea` 的 `Repeater<SplitPane>` 内（分屏多实例，
且跨 3 个 shell）。直接拿「当前活动 pane 的 mindmapPane」在 Repeater 下很别扭。改为：
`Views.requestSaveMindmap()`（C++ `ViewArea`，镜像既有 `requestInsert`/`insertText` 模式）发
`saveMindmapRequested` 信号；每个 `MindmapPane` 经 `Connections` 监听，`saveToNote()` 内 **visible 守卫**
确保只有 mindmap 模式下可见的那个 pane 真正回写。零 shell 改动、天然支持分屏。

## 限制（MVP）

- 仅处理 **标题结构**（节点文本 = 标题文本）；节点备注/样式/折叠状态不回写。
- 编辑交互（增删节点后 Ctrl+S）属会话内行为，不在 harness 跑；**往返序列化** 为可自动验证核心。
- `.mm`/`.json` 独立文件双向、导出 PNG/SVG、节点链接跳转 → 配置/后续，不另派生 spec（思维导图族收口）。

## 涉及文件

- `src/data/web/mindmap.html` — `renderMindmap(md,editable)` + `treeToMarkdown` + `window.mindmapMarkdown()` + `lastData`。
- `src/qml/shell/MindmapPane.qml` — `editable`/`bufferId` 属性 + `saveToNote()` + Ctrl+S `Shortcut` + `Views` 信号 `Connections`。
- `src/qml/shell/EditorArea.qml` — mindmap 模式 MindmapPane 设 `editable: true` + `bufferId`。
- `src/qml/shell/UnitedEntry.qml` — 命令「保存思维导图到笔记」。
- `src/widgets/viewarea/viewarea.{h,cpp}` — `requestSaveMindmap()` / `saveMindmapRequested()`。
- `src/i18n/markly_en_US.{ts,qm}` — “Save mind map to note”。
