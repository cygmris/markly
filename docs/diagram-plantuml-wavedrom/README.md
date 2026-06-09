# WaveDrom + PlantUML 图表 — spec #9f

补全预览图表的最后两类：**WaveDrom**（时序波形图，客户端渲染）与 **PlantUML**（UML 图，
编码后走公共服务器渲染）。复用 #9d/#9e 的 `renderDiagrams()` 钩子，离线打包对应 JS。
至此预览图表覆盖 **Mermaid / Graphviz / Flowchart / WaveDrom / PlantUML** 五类，与 vnote 对齐。

## 组成

| 文件 | 改动 |
| --- | --- |
| `src/data/web/wavedrom.theme-default.js` + `wavedrom.min.js` | WaveDrom（全局 `WaveDrom`） |
| `src/data/web/plantuml.synchro2.js` + `plantuml.zopfli.raw.min.js` | PlantUML 编码器（`encode64_` + `Zopfli`） |
| `src/data/web/preview.html` | `renderWavedrom()` + `renderPlantuml()` + `plantumlUrl()`；并入 `renderDiagrams()` |
| `src/data/web/preview.css` | `.wavedrom-graph` / `.plantuml-graph` |
| `src/CMakeLists.txt` | 4 个 JS 入 `markly_web` QRC |

## WaveDrom（客户端）

遍历 `pre > code.language-wavedrom, .language-wave`：先把代码块替换为 `div.wavedrom-graph`
（容器须在文档中），`WaveDrom.RenderWaveForm(idx, eval('(' + src + ')'), 'mk-wave-')` 同步
渲染为内联 SVG。源是 JS 对象字面量（用 `eval` 解析，与 vnote 一致；仅预览用户自己的笔记）。
失败 → `div.diagram-error`；`window.WaveDrom` 缺失则跳过。

## PlantUML（服务器渲染）

`plantumlUrl(src)`：源码 `unescape(encodeURIComponent(...))` → 字节数组 →
`Zopfli.RawDeflate(...).compress()` → 自定义 base64 `encode64_(...)` →
`https://www.plantuml.com/plantuml/svg/<encoded>`。`renderPlantuml()` 把
`pre > code.language-plantuml, .language-puml` 替换为 `div.plantuml-graph > img(src=该 URL)`。
**编码在本地离线完成**；图片由公共服务器在线渲染（离线时 img 加载失败，但 URL 已正确构造）。
`window.Zopfli` / `encode64_` 缺失则跳过。

## 验证

- 构建通过；既有 **ctest 22/22 无回归**（纯前端）。
- **端到端**（throwaway QWebEngineWidgets 加载真实 `preview.html`）：`mdRender` 一个
  ` ```wave `、一个 ` ```puml `（`@startuml A->B @enduml`）、一个 ` ```js `，dump `#content`：
  - wavedrom 块 → `<div class="wavedrom-graph" id="mk-wave-0"><svg ... class="WaveDrom">`（客户端渲染）；
  - plantuml 块 → `<img src="https://www.plantuml.com/plantuml/svg/SoWkIImgAStD...">`（已编码 payload）；
  - js 块仍 Prism；`diagram-error` 数 = 0。
  - 证据见 `render-evidence.txt`。
- 注：offscreen WebEngine 不栅格化 SVG（同 #9d/#9e），以渲染后 HTML 为权威证据；测试内容用
  双引号包 wave 源以避免 `mdRender('...')` 单引号转义问题。

## 后续小迭代

自定义 PlantUML 服务器 / 本地 jar（QProcess）、图表渲染缓存、WaveDrom 主题切换。
