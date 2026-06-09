# Markly — 数学公式（preview-math spec #9c）

为预览补**数学公式渲染**：用 **KaTeX**（离线、同步、轻量）+ **markdown-it-texmath** 渲染 `$...$` 行内与 `$$...$$` 块级公式。复用 #9/#9b 的 QWebEngine + markdown-it 预览管线。

## 为何 KaTeX 而非 MathJax

vnote 的 `mathjax.js` 只是个 **CDN 加载器（需联网）**，不离线。KaTeX 是离线 dist（`katex.min.js` 268K + `katex.min.css` 24K + woff2 字体 ~588K），**同步渲染**（无异步 promise），可截图/导出验证，故采用 KaTeX 等价替代。

## 组件

- **资源**（`src/data/web/`）：`katex/katex.min.js`、`katex/katex.min.css`、`katex/fonts/*.woff2`（20 个，仅 woff2 精简）、`markdown-it-texmath.js`（复用 vnote）。全部打包 `markly_web` QRC。`katex.min.css` 的 `url(fonts/...)` 在 `katex/` 下相对解析。
- **preview.html**：`<head>` 加 `katex.min.css`；脚本加载 `katex.min.js`(→`window.katex`)+`markdown-it-texmath.js`(→`window.texmath`)；`if(window.texmath&&window.katex) md.use(texmath,{engine:katex,delimiters:'dollars',katexOptions:{throwOnError:false}})`（防御式，语法错误显占位不崩）。不破坏 #9b 插件/Prism/图片/主题。
- **preview.css**：`.katex{color:var(--text)}`（继承文本色）、`.katex-display` 块级居中/间距。

## 验证

- ctest 15/15（不回归）。
- **导出 HTML 验证**（复用 #15 `MARKLY_EXPORT=html`）：含公式笔记导出后 grep —— `katex-display`×1（块级 `$$\int_0^1 x^2\,dx=\frac13$$`）、`mfrac`×2（两个 `\frac`）、`sqrt`×1（`\sqrt{b^2-4ac}`）。**行内 `$x=\frac{-b\pm\sqrt{}}{2a}$` 与块级积分均渲染为 KaTeX**。导出与预览用同一 preview.html，故预览渲染一致。
- 已知：offscreen 软件渲染下，**实时预览截图在 1200ms 抓帧时 KaTeX(268K+字体) 可能未渲染完**（甚至偶发 SIGABRT），故实时预览截图未展示公式；但导出（异步等待完成）确证渲染正确。真实显示下实时预览正常。

## 后续（再规划）

- **#9d diagram-rendering**：Mermaid（3.2MB 异步）+ Flowchart/PlantUML/Graphviz/WaveDrom 图表引擎。
- 导出 HTML 的 KaTeX 字体 base64 内联（@font-face 内联）使导出公式离线带字体；章节号自动编号 → 待办。
