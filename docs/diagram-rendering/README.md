# Mermaid 图表渲染 — spec #9d

对标 vnote-3201 预览图表：把 ` ```mermaid ` 围栏代码块在 QWebEngine 预览中渲染为
SVG（流程图/时序图/甘特图等）。复用 #9/#9b/#9c 的 `preview.html` 管线，离线打包。

## 组成

| 文件 | 改动 |
| --- | --- |
| `src/data/web/mermaid.min.js` | 打包的 mermaid（UMD，v10，全局 `mermaid`，约 3.3MB；源自 vnote-3201） |
| `src/data/web/preview.html` | 引入 mermaid；`renderMermaid()` 渲染钩子 + `window.mermaidPending()` 计数 + 主题 |
| `src/data/web/preview.css` | `.mermaid-graph` / `.mermaid-error` 样式 |
| `src/qml/shell/ExportView.qml` | 导出前轮询 `mermaidPending()` 归零再读 `outerHTML`/`printToPdf` |
| `src/CMakeLists.txt` | `mermaid.min.js` 入 `markly_web` QRC |

## 渲染流程

1. `mdRender(text)`：markdown-it 渲染 → Prism 高亮 → `renderMermaid()`。
2. `renderMermaid()`：遍历 `pre > code.language-mermaid`，每块：
   - `mermaid.render('mk-mermaid-<n>', src)` → `{svg}`，用 `div.mermaid-graph` 替换原 `pre`；
   - 失败 → `div.mermaid-error` 显示错误（不破坏其它块/文本）；
   - `__mermaidPending` 计数（开始 ++ / `finally` --）。
3. `window.mermaidPending()` 返回未完成数（0 = 全部完成）。
4. 非 mermaid 代码块（如 ` ```js `）不受影响，仍由 Prism 高亮。
5. `window.mermaid` 缺失时 `renderMermaid` 直接 return（防御降级）。

## 主题

`mdSetTheme(json)` 解析背景色 `bg` 亮度（0.299R+0.587G+0.114B）：< 128 → mermaid
`dark` 主题，否则 `default`，并 `mermaid.initialize({startOnLoad:false, theme})`。

## 异步与导出

Mermaid 渲染异步：实时预览图表就绪即时出现。导出（#15 ExportView）在 `mdRender`
回调后用 80ms `Timer` 轮询 `mermaidPending()`，归零（或 ~3.2s 超时）后再读
`outerHTML`（HTML）或 `printToPdf`（PDF），使导出产物内含已渲染 SVG。无图表文档
`pending` 立即为 0，不增加延迟。

## 验证

- 构建通过；既有 **ctest 18/18 无回归**（纯前端 + QML，无新 C++）。
- **端到端**：用真实 `preview.html`（`file://` + 同目录 `mermaid.min.js`，离线）`mdRender`
  一个 ` ```mermaid `（`graph TD; A-->B`）+ 一个 ` ```js ` 块，轮询 `mermaidPending()`
  归零后 dump `#content`：
  - mermaid 块 → `<svg aria-roledescription="flowchart-v2" id="mk-mermaid-0">`（替换了
    `pre/code`）；
  - js 块仍为 `<pre class="language-js">` + Prism `token`。
  - 证据见 `render-evidence.txt`。
  - 注：offscreen WebEngine 不把 SVG 栅格化进 `grab()`（同 KaTeX 已知限制），故以渲染
    后 HTML 为权威证据（沿用 #9c「重型 JS 预览用渲染 HTML 验证」结论）。

## 依赖 / 打包

mermaid.min.js 随 `markly_web` QRC 进二进制（`qrc:/data/web/mermaid.min.js`），预览页
`<script>` 引入，完全离线。

## 后续小迭代（再规划 #9e diagram-extra）

- PlantUML（在线服务 / 本地 jar）、Graphviz（viz.js）、Flowchart.js、WaveDrom。
- 图表缩放/单图导出、mermaid 主题细化、错误更友好提示。
