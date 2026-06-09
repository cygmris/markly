# Graphviz + Flowchart.js 图表 — spec #9e

在 #9d（Mermaid）基础上补充客户端 **Graphviz（viz.js）** 与 **Flowchart.js**，复用同一
渲染钩子模式、统一待渲染计数与 #15 导出等待，离线打包。

## 组成

| 文件 | 改动 |
| --- | --- |
| `src/data/web/viz.js` + `viz.lite.render.js` | Graphviz（viz.js，全局 `Viz`，约 1.45MB；源自 vnote-3201） |
| `src/data/web/flowchart.min.js` + `raphael.min.js` | Flowchart.js（全局 `flowchart`，依赖 Raphael） |
| `src/data/web/preview.html` | `renderGraphviz()` / `renderFlowchart()` + 统一 `renderDiagrams()`；计数泛化 |
| `src/data/web/preview.css` | `.graphviz-graph` / `.flowchart-graph` / `.diagram-error` |
| `src/CMakeLists.txt` | 4 个 JS 入 `markly_web` QRC |

## 渲染流程

`mdRender` → markdown-it → Prism → `renderDiagrams()` = `renderMermaid()`（#9d）+
`renderGraphviz()` + `renderFlowchart()`。

- **Graphviz**：遍历 `pre > code.language-dot, .language-graphviz`，`new Viz().renderSVGElement(src)`
  （异步）→ `div.graphviz-graph` 包 SVG 替换 `pre`；失败 → `div.diagram-error` 并重建 Viz
  （viz.js 单次错误后失效，需重建供后续块）。
- **Flowchart.js**：遍历 `pre > code.language-flow, .language-flowchart`，先替换为空
  `div.flowchart-graph`（drawSVG 需在文档中的容器），`flowchart.parse(src).drawSVG(div)`
  （同步）；失败 → `div.diagram-error`。

## 统一计数（关键）

#9d 的 `__mermaidPending` 泛化为 `__diagramPending`，三引擎共用；`window.mermaidPending()`
（名字为 #15 ExportView 兼容保留）现返回**所有图表**未完成数。因此 **ExportView 无需改动**
即等待 graphviz/flowchart 完成后再读 `outerHTML`/`printToPdf`。id 序列 `__diagramSeq` 共用。

## 防御

任一库缺失（`window.Viz` / `window.flowchart`）对应渲染器直接 return；单块失败仅显示
该块错误占位，不影响其它块、文本或其它引擎；非图表代码块仍由 Prism 高亮。

## 验证

- 构建通过；既有 **ctest 18/18 无回归**（纯前端）。
- **端到端**（throwaway QWebEngineWidgets 加载真实 `preview.html`，离线）：`mdRender`
  一个 ` ```dot `（`digraph{A->B;B->C}`）+ 一个 ` ```flow ` + 一个 ` ```js `，轮询
  `mermaidPending()` 归零后 dump `#content`：
  - graphviz 块 → `<div class="graphviz-graph"><svg ... width="62pt" ...>`（真实 Graphviz 输出）；
  - flowchart 块 → `<div class="flowchart-graph"><svg ...>`（Raphael）；
  - js 块仍 `language-js` + Prism；`diagram-error` 数 = 0。
  - 证据见 `render-evidence.txt`。
- offscreen WebEngine 不栅格化 SVG 进 `grab()`（同 #9d/#9c 已知限制），以渲染后 HTML 为
  权威证据。

## 后续小迭代（再规划 #9f）

- PlantUML（在线 server / 本地 jar）、WaveDrom（皮肤脚本）。
- DOT/flow 块的 Prism 跳过优化、图表缩放/单图导出。
