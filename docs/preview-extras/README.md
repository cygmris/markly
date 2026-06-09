# Markly — 预览增强与大纲（preview-extras spec #9b）

在 #9 markdown-preview 之上丰富预览渲染（markdown-it 插件 + Prism 代码高亮）并补**真实大纲**。复用源 `vnote-3201/src/data/extra/web/js/` 资源。

## 再规划：重型引擎拆 #9c

MathJax/Mermaid(3.2MB)/Flowchart/PlantUML/Graphviz/WaveDrom 体量大、各有独立集成关切，已拆为新 spec **#9c diagram-math-extras**（PLANNED）。本 spec 只做离线、轻量、可靠的增强。

## A. 预览渲染增强

- **markdown-it 插件**（打包 `:/data/web/`，preview.html 防御式 `md.use`）：footnote、sub、sup、mark、emoji、task-lists（`{label:true}`）、anchor、toc-done-right。缺失插件不致崩（usePlugin try/catch）。
- **Prism**：`prism.min.js`，`mdRender` 后 `Prism.highlightAllUnder(content)`；markdown-it fence 默认输出 `language-xxx` 类供 Prism 识别。
- **preview.css**：Prism token 配色用 CSS 变量（`--tok-keyword/string/comment/number/func/operator/tag`）+ 任务清单复选框/脚注/sub/sup/mark/emoji/anchor 样式。
- **PreviewPane.qml**：`pushTheme` 增注 Prism token 颜色（从 Theme 角色 pink/sage/dim/amber/link/text/accent 取，`hex()` 转 #rrggbb），随主题重注入。

## B. 真实大纲（outline）

- **ViewArea**（不新建实体）：`Q_PROPERTY outline`（`[{level,text,line}]`，1 基行号）解析当前激活 buffer 的 markdown 标题（`^#{1,6}\s+`），**跟踪围栏代码块（```/~~~）跳过其中的 `#`**。`connect(changed → outlineChanged)` 使其随视图/编辑更新。
- **即时跳转**：`gotoOutlineLine(line)` → 新信号 `gotoLineNow(line)`；`MarkdownEditor` 用 `Connections{ target: Views; onGotoLineNow → gotoLine }` 立即跳转当前编辑器——**同时补 #8「点击已激活 buffer 的结果不跳转」缺口**（提供能力；搜索是否改用由后续决定）。
- **三外壳**：A `ShellRefined` 与 C `ShellWorkbench` 大纲面板 Repeater 接 `Views.outline`（替换 DemoData），点击 `Views.gotoOutlineLine(line)`；B `ShellFocus` 为沉浸写作无大纲面板（不变）。`OutlineRow` 加 `clicked` 信号 + hover。

## 验证

- ctest 11/11（`test_bufferview` 加 outline 用例：多级标题解析、代码块内 `#` 排除、updateText 后更新）。
- 截图：read 模式 `/tmp/markly-shots/preview9b-read.png` —— 任务清单复选框（☑☑☐）、Prism 代码高亮（int/return 关键词、add 函数、// 注释 token 上色，主题驱动）、accent 标题；右侧大纲列出真实标题。split 模式亦验证。
- offscreen 退出 WebEngine segfault(139) 仍为 #9 既有现象，截图先存不受影响。

## 后续（再规划）

- **#9c diagram-math-extras**：MathJax、Mermaid、Flowchart、PlantUML、Graphviz(viz.js)、WaveDrom。
- 章节号自动编号、user.css、编辑↔预览双向滚动同步、大纲当前标题高亮跟随光标 → polish 或 #9c。
- 搜索结果对已打开文件改用 `gotoLineNow` 即时跳转（能力已具备）→ 小迭代。
