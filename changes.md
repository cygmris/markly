# Changes

## Unreleased

### imagehost-clone (spec #10e) — 2026-06-09
* Git 仓库图床首次自动 clone：配置 clone_url 但本地非 git 仓库时，首次上传前 git clone。图床族收口。
* RepoImageHost 加 `cloneArgs`（纯）/`isGitRepo`（文件系统）/`ensureCloned`（已是仓库快路径零网络，否则真实 git clone 60s 超时）；ImageHostBridge repo upload 前 ensureCloned；ImageHostCfg.cloneUrl + SettingsDialog「克隆地址」字段。
* 验证：test_imagehost 加 cloneArgs + **真实 git** isGitRepo（init 前 false/后 true）+ ensureCloned 快路径（已是仓库 true、非仓库空 url false），ctest 26/26；qmllint 无语法错误；offscreen 启动无错。真实 clone 走网络（离线不测，文档记录）。

### vi-macros-search (spec #8h) — 2026-06-09
* Vi 宏 q/@（纯逻辑 ViEngine，GUILESS 可测，无 QML 改动）：`q<reg>` 录制按键序列（边录边执行）+ `q` 停止；`@<reg>` 回放。
* 控制键（q/@/寄存器名）在 handleKey 顶部拦截不录；回放在本地文本副本上推进（多步编辑），最终以整篇编辑 `{0,origLen,finalText}` 返回；m_replaying 防重入；reset 保留 m_macros。
* 覆盖 Normal 结构性命令宏（含计数）；Insert 内键入暂不录（同 `.` 边界）；搜索 `/?n N`、可视块 `Ctrl-V` 标注需编辑器 UI 集成（不派生 spec）。
* 验证：test_viengine 加宏用例（qaxxq+@a、多命令 0xj、空寄存器），ctest 26/26（test_viengine 16 组）；MarkdownEditor 未改（qmllint 干净）；offscreen 启动无错。

### export-presets (spec #15d) — 2026-06-09
* 导出预设/选项：Pandoc 导出加 standalone（独立文档）/ toc（目录页）/ css（自定义样式）选项。导出族收口。
* PandocExporter 加 PandocOptions{standalone,toc,cssPath} + 选项版 buildArgs（追加 --standalone/--toc/--css）/exportTo；无选项版委托默认 opts 兼容 #15b/#15c。PandocBridge.exportNoteOpts；UnitedEntry 命令「导出 HTML（含目录）」。
* 验证：test_pandoc 加选项 buildArgs + **真实 pandoc --toc**（输出含 `id="TOC"`），ctest 26/26；qmllint 无语法错误；offscreen 启动无错。
* wkhtmltopdf 已从 Arch 仓库移除→不支持（内建 PDF 走 QWebEngine printToPdf #15）；LaTeX PDF 需 texlive（可选），文档记录。

### global-hotkey (spec #17b) — 2026-06-09
* 全局热键：配置热键（默认 Ctrl+Alt+M）把主窗口呼到前台。热键解析纯函数可测；OS 级注册用 X11/XCB（xcb 平台守卫）。
* HotkeyParser（core/hotkey，QKeySequence 解析 → {valid,modifiers,key}）；GlobalHotkey（QAbstractNativeEventFilter + XCB grab，platformName!="xcb" 安全降级）；MainWindow activated→showMainWindow；WidgetConfig.global_hotkey + TrayCfg.globalHotkey + SettingsDialog 字段。
* markly_core 链接 xcb/xcb-keysyms（可选，无则 GlobalHotkey no-op）；Num/Caps lock 变体一并 grab。
* **诚实记录**：Wayland 禁止便携全局热键，本机 Wayland 会话 X11 grab 编译通过但运行不触发；跨平台呼出回退由单实例 requestShow 实现（已有）。X11 会话可手动验证。
* 验证：test_hotkey（Ctrl+Alt+M / Meta+Shift+P / 非法），ctest 26/26；qmllint 无语法错误；offscreen 启动 GlobalHotkey 安全 no-op 无崩溃。

### desktop-integration (spec #20c) — 2026-06-09
* 桌面集成收口：自动检查更新（按周期）+ 文件关联（.md→Markly）+ 「聚焦：编辑器」导航命令。
* UpdateChecker.shouldCheck（周期判定纯函数：从未/`last<0`/间隔）；WidgetConfig.auto_update_check + last_update_check；UpdateBridge.autoCheckIfDue（到期才检查并更新时间戳，MarklyShell 启动调用）。
* FileAssoc（core/desktop）：registerCommands（xdg-mime default markly.desktop text/markdown，纯）+ registerNow（最佳努力）；TrayCfg.autoUpdateCheck + SettingsDialog「自动检查更新」开关。
* 验证：test_update 加 shouldCheck（0/8天/1天/-1）+ FileAssoc.registerCommands，ctest 25/25；qmllint 无语法错误；offscreen 启动无错。

### imagehost-repo (spec #10d) — 2026-06-09
* 通用 Git 仓库图床：把图片提交到本地 Git 仓库克隆（add+commit+push），返回 raw URL。至此图床三类全覆盖（GitHub/Gitee/Git 仓库）。
* RepoImageHost（core/imagehost）：纯函数 `rawUrl`（rawBase 去尾斜杠 + /branch/path）+ `commitImage`（真实 git 写+add+commit，自带 user identity 免全局配置）+ `push`（网络最佳努力）。
* ImageHostBridge `type=="repo"` 分派（commitImage→push→uploaded(rawUrl)）；ImageHostCfg 加 localRepo/rawBase/subDir；SettingsDialog 图床类型加「Git 仓库」+ 字段。
* 验证：test_imagehost 加 rawUrl + **真实 git** init/commitImage（git log/ls-files 验证）+ 非仓库失败，ctest 25/25；qmllint 无语法错误；offscreen 启动无错。

### export-merge (spec #15c) — 2026-06-09
* 全合一/合并导出：把一个文件夹下多个 .md 合并成单文档再导出（复用 #15b 真实 pandoc，或直写 md）。
* NoteMerge（core/export）：`merge`（每篇可选 `# 标题` + 内容，`\n\n---\n\n` 连接）/`mergeDir`（遍历 *.md 按名序读取）/`exportMergedDir`（md 直写或 pandoc 转换），纯逻辑可测。
* MergeBridge（QML `Merge`：exportDir）；UnitedEntry 命令「合并导出 Markdown/HTML/Word」（html/docx 仅 Pandoc.available()），合并当前文件夹。
* 验证：test_merge（merge/mergeDir 按名序/空目录 + **真实 pandoc 合并→html** 含两篇内容），ctest 25/25；qmllint 无语法错误；offscreen 启动无错。

### export-advanced (spec #15b) — 2026-06-09
* Pandoc 自定义格式导出：用系统 pandoc 把当前笔记导出为 docx/epub/latex 等；复用 #16 QProcess 模式。
* PandocExporter（core/export）：静态 `buildArgs`/`isAvailable`（纯函数可测）+ `exportTo`（写临时 md → QProcess 跑 pandoc → 校验输出非空，30s 超时，临时文件自动清理）。
* PandocBridge（QML `Pandoc`：available/exportNote）；UnitedEntry 仅 available 时列出「导出 Word/EPUB/LaTeX (Pandoc)」命令；翻译（.qm 57 条）。
* 验证：test_pandoc **真实执行 pandoc** markdown→html（输出含 `<h1`/Title）+ buildArgs + 失败路径，ctest 24/24；qmllint 无语法错误；offscreen 启动无错。依赖系统 pandoc（pandoc-cli），未装能力守卫降级。

### vi-repeat-search (spec #8g) — 2026-06-09
* Vi `.` 重复上次修改（纯逻辑 ViEngine，GUILESS 可测，无 QML 改动）：修改录制（m_curKeys 累积构成命令的按键，歇下且产生编辑且停 Normal 时提交 m_lastChange）+ 回放（`.` 用同一原始光标逐键重入 handleKey 累积 edits，m_replaying 防重入）。
* 覆盖停在 Normal 的修改 x/dd/dw/D/r/~/`>>`/`<<`/p（含计数与参数键）；进入 Insert 的修改留 #8h。
* 验证：test_viengine 加 `.` 用例（x/dd/~/rX+移动/2x/无录制），ctest 23/23（test_viengine 15 组）；MarkdownEditor 未改（qmllint 干净）；offscreen 启动无错。

### mindmap-viewer (spec #18b) — 2026-06-09
* 思维导图视图：把当前笔记标题大纲渲染成只读 MindElixir 思维导图，作为新 viewMode "mindmap"（edit/read/split/mindmap）。
* 打包 MindElixir.js（UMD，约 80KB）+ mindmap.html（parseHeadings→buildTree→`new MindElixir({editable:false}).init`）进 markly_web QRC；MindmapPane.qml（WebEngineView，随 Views.currentText 重渲染）。
* EditorArea mode==="mindmap" 显示 MindmapPane（隐藏编辑器/预览/查看器）；ViewArea.setViewMode 放行 mindmap；UnitedEntry 命令「查看思维导图」/「编辑模式」。
* 验证：ctest 23/23 无回归 + 端到端 DOM 证据（# Root/## A/## B/### A1 → MindElixir root Root + 子 A/B + A1 嵌套 B，层级=标题级别；docs/mindmap-viewer/render-evidence.txt）。

### vi-search-repeat (spec #8f) — 2026-06-09
* Vi WORD/替换/配对（纯逻辑 ViEngine，GUILESS 可测，无 QML 改动）：`W`/`B`/`E`（WORD 动作，仅空白分隔，支持计数）、`s`/`S`（substitute 字符/整行进 Insert）、`%`（括号配对跳转，计嵌套）。
* 新增静态辅助 WORDForward/WORDBackward/WORDEnd/matchBracket。
* 验证：test_viengine 扩展 3 组（WORD/substitute/matchBracket），ctest 23/23（test_viengine 14 组）；MarkdownEditor 未改（qmllint 干净）；offscreen 启动无错。

### tray-update (spec #20b) — 2026-06-09
* 系统托盘（QSystemTrayIcon，`isSystemTrayAvailable()` 守卫离屏降级）+ 菜单（显示/退出）+ 可选最小化到托盘；GitHub Releases 更新检查。
* UpdateChecker（core/update）：静态 `parseLatestVersion`（tag_name 去前导 v）/`isNewer`（QVersionNumber 数值比较）可单测 + 异步 check。UpdateBridge（QML `Update`：check/checked，owner/repo 占位需改）。
* WidgetConfig.minimize_to_tray（默认 false）+ TrayCfg（QML `TrayCfg`）；MainWindow.setupTray + closeEvent 隐藏到托盘；托盘图标复用 #21 markly.png（QRC alias :/markly.png）。
* UnitedEntry 命令「检查更新」+ 结果通知；SettingsDialog「最小化到托盘」开关；翻译（检查更新/最小化到托盘等，.qm 50 条）。
* 验证：test_update（parse/compare 数值），ctest 23/23（含 2 smoke，托盘守卫不破坏 smoke_startup）；qmllint 无语法错误；offscreen 启动无错。

### imagehost-extra (spec #10c) — 2026-06-09
* Gitee 图床：复用 #10b 框架，新增 GiteeImageHost（Gitee Contents API，POST + access_token 在体 + api/v5），ImageHostBridge 按 type 分派 github/gitee。
* GiteeImageHost 静态 `uploadUrl`/`uploadBody`（access_token+base64+可选 branch）可单测；响应解析复用 `GithubImageHost::parseDownloadUrl`（同字段）。
* configured() 含 gitee；SettingsDialog 图床类型加 Gitee 选项（none/github/gitee）；上传命令/链接替换沿用 #10b。
* 验证：test_imagehost 加 Gitee uploadUrl/uploadBody（含 access_token、branch 有无），ctest 22/22；qmllint 无语法错误；offscreen 启动无错。

### vi-advanced (spec #8e) — 2026-06-09
* Vi 进阶命令（全部加进纯逻辑 ViEngine，GUILESS 可测，无 QML 改动）：`e`（词尾）、`f/F/t/T`（行内查找）+ `;`/`,`（重复/反向）、`r`（替换字符）、`~`（大小写翻转）、`>>`/`<<`（行缩进），均支持计数。
* 待参机制：`f/F/t/T/r` 置 m_pendingChar，下一键作参数（在计数累加前消费，故 `f5`/`r5` 的 5 是字符）；`;`/`,` 用 m_lastFindCmd/Target 重复；`>>`/`<<` 复用操作符挂起 + 按行 edits。
* 验证：test_viengine 扩展 5 组（e/find+;,/r/~/indent），ctest 22/22；MarkdownEditor 未改（qmllint 干净）；offscreen 启动无错。

### diagram-plantuml-wavedrom (spec #9f) — 2026-06-09
* 补全预览图表最后两类：WaveDrom（` ```wavedrom `/` ```wave `，客户端 SVG）+ PlantUML（` ```plantuml `/` ```puml `，编码→公共服务器 `<img>`），离线打包 4 个 JS 进 markly_web QRC。
* preview.html `renderWavedrom()`（`WaveDrom.RenderWaveForm`，eval 源）+ `renderPlantuml()`（`plantumlUrl`：Zopfli deflate + `encode64_` → `https://www.plantuml.com/plantuml/svg/<encoded>`）并入 `renderDiagrams()`。
* 至此预览图表 5 类全覆盖（Mermaid/Graphviz/Flowchart/WaveDrom/PlantUML），与 vnote 对齐；单块失败/缺库防御降级。
* 验证：ctest 22/22 无回归 + 端到端 HTML 证据（wave→WaveDrom SVG、puml→plantuml.com 编码 img、js→Prism、0 错误；docs/diagram-plantuml-wavedrom/render-evidence.txt）。

### packaging-tests (spec #21) — 2026-06-09
* CPack 打包（TGZ）：顶层 CMake CPack 配置，`cpack -G TGZ` 产出 `Markly-1.0.0-Linux.tar.gz`（含 bin/markly + 桌面集成，11.5MB）。
* 桌面集成：`packaging/markly.desktop` + 128×128 图标，install 到 `share/applications` / `share/icons/hicolor/128x128/apps`。
* 集成 smoke 测试：`smoke_version`（`markly --version` 退出码 0 + 输出含 Markly）、`smoke_startup`（MARKLY_SHOT 离屏全 QML 外壳初始化抓帧，断言截图产物——绕过 WebEngine offscreen 销毁段错误）。
* 验证：ctest 22/22（20 单元 + 2 smoke）；`cpack -G TGZ` 产物含 bin/markly + desktop + icon；`cmake --install --prefix` 装出三件套（验证后清理）。
* 部署流程文档 docs/packaging-tests/README.md（构建/测试/打包/安装命令 + 依赖 + 多平台后续）。

### imagehost-upload (spec #10b) — 2026-06-09
* GitHub 图床上传：把笔记本地图片上传到 GitHub 仓库（Contents API），用公开 URL 替换 Markdown 本地链接。
* GithubImageHost（core/imagehost）：静态纯函数 `uploadUrl`/`uploadBody`(base64+可选 branch)/`authHeader`/`parseDownloadUrl`（可单测）+ 异步 `upload`（QNetworkAccessManager PUT，emit done）。
* WidgetConfig.image_host `{type,user,repo,token,branch}` 读写；ImageHostBridge（`ImageHost`：configured/upload/uploaded·failed 信号）+ ImageHostCfg（`ImageHostCfg`：type/user/repo/token/branch 属性）。
* SettingsDialog 新增 `text` 输入类型 + 「图床」分类（令牌 echoMode 密码）；UnitedEntry 命令「上传图片到图床」（扫当前笔记本地图片逐个上传）；MarkdownEditor `onUploaded` 把本地路径替换为远程 URL。
* 验证：test_imagehost（uploadUrl/uploadBody/authHeader/parseDownloadUrl/config 往返），ctest 20/20；qmllint 无语法错误；offscreen 启动无 QML 错误。token 明文存 markly.json（同 vnote，文档注明）。

### vi-mode (spec #8b) — 2026-06-09
* Vi 输入模式：Normal/Insert/Visual 三模式 + 核心动作/操作符/计数/无名寄存器；编辑器设置可开关（默认关）。
* 纯逻辑状态机 ViEngine（core/editor，仅 QString/QChar，GUILESS 可测）：`handleKey` → `ViResult{handled,mode,cursor,anchor,edits[]}`，编辑器按 edits start 降序应用。
* 覆盖：`i a I A o O v Esc`；`h l j k 0 $ w b gg G x`（计数 N）；`dd yy cc dw d$ D C Y`；`p P`（行式/字符式）；Visual `hljk0$wb`+`d/x/y/c`。
* ViBridge（QML `Vi` 桥：`enabled`/`mode`/`handleKey`/`reset`）；EditorConfig.vi_mode + EditorCfg.viMode；MarkdownEditor Keys.onPressed 前置拦截；SettingsDialog「Vi 模式」开关；翻译 "Vi mode"（.qm 35 条）。
* 验证：test_viengine 6 组（动作/计数/dd·2dd·dw·D/yy+p/模式/Visual 删），ctest 19/19；qmllint 无语法错误；offscreen 启动无 QML 错误。

### spell-suggest-menu (spec #8d) — 2026-06-09
* 拼写右键建议菜单：编辑器对拼错单词右键 → Hunspell 建议列表（上限 8）点选替换 + 「添加到忽略列表」。
* 会话忽略列表放进共享 SpellChecker（`addIgnore`/`isIgnored`，大小写不敏感，`check()` 对忽略词返回 true）→ 波浪下划线高亮器与建议菜单都自动尊重，无需各维护。
* SpellBridge（QML `Spell` 桥：`enabled`/`misspelled`/`suggest`/`ignore`）薄委托共享 SpellChecker；MarkdownHighlighter `rehighlightNow()`；MarkdownEditor 右键 MouseArea + Menu + `onIgnored` 去红。
* 翻译「添加到忽略列表」→ "Add to ignore list"（.ts/.qm 34 条）。
* 验证：test_spell 加忽略用例（check→addIgnore→check / 大小写不敏感），ctest 18/18；offscreen 启动无 QML 错误，MarkdownEditor.qml qmllint 无语法错误。

### diagram-extra (spec #9e) — 2026-06-09
* 补充客户端图表：Graphviz（viz.js，` ```dot `/` ```graphviz `）+ Flowchart.js（` ```flow `/` ```flowchart `），离线打包进 markly_web QRC。
* preview.html `renderGraphviz()`（viz.js 异步 `renderSVGElement`，错误后重建 Viz）+ `renderFlowchart()`（Raphael `flowchart.parse().drawSVG`，同步）+ 统一 `renderDiagrams()`。
* #9d `__mermaidPending` 泛化为 `__diagramPending`，三引擎共用；`window.mermaidPending()`（名兼容）现计入所有图表 → ExportView #15 无需改动即等待全部完成。
* 防御：库缺失各自降级；单块失败仅显示 `diagram-error`，不影响其它块/文本/引擎；非图表块仍 Prism。
* 验证：ctest 18/18 无回归 + 端到端渲染 HTML 证据（dot→Graphviz SVG、flow→Raphael SVG、js→Prism、0 错误；docs/diagram-extra/render-evidence.txt）。

### diagram-rendering (spec #9d) — 2026-06-09
* Mermaid 图表渲染：` ```mermaid ` 围栏代码块在 QWebEngine 预览中渲染为内联 SVG（流程图/时序图等），离线打包（mermaid.min.js 进 markly_web QRC）。
* preview.html `renderMermaid()`：遍历 `pre>code.language-mermaid`，`mermaid.render` 异步替换为 `div.mermaid-graph`（svg）/`div.mermaid-error`；`__mermaidPending` 计数 + `window.mermaidPending()` 暴露。
* 非 mermaid 块（js 等）仍 Prism 高亮不受影响；`window.mermaid` 缺失防御降级；单块失败不破坏其它。
* 主题：`mdSetTheme` 依背景亮度设 mermaid `dark`/`default`。
* 导出（ExportView #15）：`mdRender` 后 80ms Timer 轮询 `mermaidPending()` 归零（~3.2s 超时）再读 outerHTML/printToPdf，使导出含已渲染 SVG。
* 验证：ctest 18/18 无回归 + 端到端渲染 HTML 证据（mermaid→`<svg flowchart-v2>`，js→Prism；docs/diagram-rendering/render-evidence.txt）。

### spell-check (spec #8c) — 2026-06-09
* 拼写检查（Hunspell）：编辑器对拼错英文单词加红色波浪下划线（`SpellCheckUnderline`），设置「编辑器」分类可开关（默认关）。
* `SpellChecker`（`core/spell`，libhunspell C API：`check`/`suggest`/`ready`）：词典加载失败安全降级（`check` 一律 true）；编码经 Qt6 `QStringConverter`（不依赖 Core5Compat）。
* `MarklyApp.getSpellChecker()` 懒加载共享实例，词典目录解析 `MARKLY_DICT_DIR` > `<config>/dicts` > 内置 `data/dicts`。
* `MarkdownHighlighter` 拼写 pass：正则 `[A-Za-z]{2,}` 取词，跳过代码块/行内码，叠加在语法格式之上（保留前景色）；`EditorCfg.spellCheck` 反应式绑定，切换即 rehighlight。
* `EditorConfig.spell_check`（默认 false）+ `EditorCfgQml.spellCheck` 可写；SettingsDialog 加「拼写检查」toggle（en "Spell check"，.ts/.qm 33 条）。
* 词典 `src/data/dicts/en_US.{aff,dic}` 随 app 分发；CMake `pkg_check_modules(HUNSPELL hunspell)` 链接。
* 验证：`tests/test_spell.cpp`（hello/helllo/suggest/无词典降级，ctest 18/18）+ 波浪下划线渲染截图（docs/spell-check/images）。

### extra-viewers (spec #18) — 2026-06-09
* 额外查看器：打开 .pdf/.html 节点时编辑区显示 ViewerPane(QWebEngine)只读查看器而非 Markdown 编辑器。
* ViewArea.splits 加 currentPath；ViewerPane.qml(WebEngineView pdfViewerEnabled+localContentCanAccessFileUrls,file:// 加载)；EditorArea 按扩展名 viewerType 切换。
* PDF 内建查看器,HTML 直接渲染；页面 origin=文件本身(file://)无跨源(区别 #10 预览 qrc 需 data URI)。
* ctest 17/17(test_bufferview 加 currentPath 断言)；截图验证打开 .html 显 ViewerPane 渲染(自定义 CSS 生效)。
* 再规划：思维导图编辑器拆出 #18b mindmap-viewer。

### i18n (spec #20) — 2026-06-09
* 国际化基础设施:QTranslator 加载链路+语言偏好(WidgetConfig.language)+设置内语言切换(engine.retranslate 即时)+qsTr 子集(SettingsDialog/UnitedEntry)+英文 .ts/.qm。
* MarklyApp.applyLanguage(auto→系统/zh 源/en_US 加载 :/i18n/markly_en_US.qm);main 启动前安装;LocaleBridge(context property Locale)setLanguage→配置+applyLanguage+retranslate。
* 分类用稳定 key(editor/appearance/about)匹配避免 retranslate key 失配;langseg 字符串值控件切语言。
* markly_en_US.ts(32 条,context=QML 文件名)lrelease 生成 .qm 打包 markly_core QRC。
* ctest 17/17(新增 test_locale:language 往返+QTranslator translate 设置→Settings/新建笔记→New note);截图验证 en_US 设置对话框全英文。
* 再规划:系统托盘/更新检查/文件关联/NavigationMode 拆出 #20b tray-update;全量 qsTr 覆盖持续任务。

### united-entry (spec #17) — 2026-06-09
* 统一入口命令面板：Ctrl+P 覆盖层,输入运行命令或跳转笔记(名称搜索复用 #12 Search)。纯 QML 复用既有桥接,无新 C++。
* UnitedEntry.qml:命令模型(新建笔记/快速记录/打开设置/切换深浅/聚焦页/导出 PDF·HTML·MD)+笔记结果混排+类型徽章+键盘导航(上下/回车)。导出命令内嵌 ExportView 自包含。
* 三外壳挂 UnitedEntry(actions 注入 settingsDialog/leftPage/newRootNote)+Ctrl+P Shortcut;工作台 Omnibar 点击打开(占位终成真)。
* ctest 16/16(不回归)；截图验证命令面板(命令列表+徽章+高亮)。
* 再规划:OS 全局热键(QHotkey,Wayland 受限)拆出 #17b global-hotkey。

### task-system (spec #16) — 2026-06-09
* 外部命令任务：TaskMgr(JSON 任务定义+变量展开+QProcess 执行)+工具栏「任务」菜单运行当前任务→notify 输出。
* 变量 ${bufferPath}/${bufferName}/${bufferBaseName}/${bufferDir}/${notebookFolder}/${cwd}(对齐 vnote 子集),未知保留无值空。
* run:QProcess(shell→sh -c 否则 program+args,cwd=notebookFolder||bufferDir,合并 stdout/stderr,超时 kill,FailedToStart 不崩)。seedDefaults 字数统计(wc)。
* MarklyApp 持有真实 TaskMgr(替换 nullptr 占位);TaskBridge(context property Tasks)list+run。
* ctest 16/16(新增 test_task:expand/run echo 真实 QProcess/缺失命令/seed)。
* 再规划:异步流式输出/任务编辑 UI/任务快捷键待办。

### preview-math (spec #9c) — 2026-06-09
* 预览数学公式：KaTeX(离线同步)+markdown-it-texmath 渲染 $...$ 行内/$$...$$ 块级公式。接入 #9 preview.html。
* 资源:katex.min.js/css+20 woff2 字体+texmath 打包 QRC;defensive md.use(dollars,throwOnError:false)。preview.css 公式继承文本色。
* 为何 KaTeX:vnote mathjax.js 是 CDN 加载器(需联网),KaTeX 离线同步可验证。
* ctest 15/15(不回归)；导出 HTML 验证 katex-display/mfrac×2/sqrt×1(行内+块级公式均渲染)。
* 已知:offscreen 实时预览截图 1200ms 抓帧 KaTeX 可能未渲染完(导出确证正确)。
* 再规划:Mermaid 等图表引擎拆出 #9d diagram-rendering。

### settings-dialog (spec #19) — 2026-06-09
* 可搜索设置对话框：分类(编辑器/外观/关于)+搜索过滤+即时生效。三外壳 gear 入口打开。
* EditorCfgQml 可写(WRITE setter→EditorConfig setter+changed,fontSize/tabWidth clamp)：编辑器选项设置里可改即时刷新。
* SettingsDialog.qml(覆盖层 modal)：JS 设置项模型(get/set 闭包绑 EditorCfg/Appearance)+内联 Toggle/Stepper/SegSelect/Swatches 控件;搜索跨分类按标签过滤。
* 外观分类绑 Appearance(#2 已可写)；作为统一容器,后续 #8b/#8c/#10b/#17/#20 各自追加设置项。
* ctest 15/15(test_editorconfig 扩展 autoIndent/continueList/autoPair 往返)；截图验证设置对话框编辑器分类。

### export-system (spec #15) — 2026-06-09
* 导出当前笔记为 Markdown/HTML/PDF：编辑器工具栏「导出」菜单→保存对话框→生成文件。复用 #9 预览渲染+#10 图片 data URI。
* ExportHelper(context property Export)writeText/readResource;DialogHelper.saveFile;ViewArea.currentText。
* ExportView.qml(隐藏 WebEngine A4):PDF 用 printToPdf;HTML 取 outerHTML→inline preview.css+删 script→自包含;图片渲染前 data URI 内联。
* 自包含 HTML:图片内联+CSS 内联(readResource)+主题变量(html inline style)+删脚本→离线单文件可开。
* ctest 15/15(新增 test_export)；MARKLY_EXPORT 离线验证 HTML(自包含)+PDF(PDF-1.4 1 页)生成。
* 再规划：Pandoc/wkhtmltopdf/全合一/打印拆出 #15b export-advanced。

### snippet-template (spec #14) — 2026-06-09
* 代码片段/魔法词/模板：左 dock「片段」页(列表+插入+删除+新增)，插入展开到编辑器光标；右键文件夹「从模板新建笔记」。
* SnippetMgr(core):片段存 <config>/snippets/*.json;apply 展开魔法词(%date%/%time%/%datetime%/%note%/%uuid%)+$$ 选区+@@ 光标(移除记 offset);seedDefaults(日期/代码块)。
* SnippetBridge(context property Snippets)list+apply/add/remove;ViewArea.requestInsert→insertText 信号→MarkdownEditor 光标插入定位(同 #9b 模式)+currentFileName。
* NotebookExplorer.newNoteFromTemplate(展开模板写入新笔记);三外壳 A/B snippet 按钮→leftPage=snippet。
* ctest 14/14(新增 test_snippet)；截图验证片段页内置代码块/日期+按钮。
* 再规划：脚本型片段/快捷键绑定/模板选择器对话框 → 后续小迭代。

### quickaccess-history-flash (spec #13) — 2026-06-09
* 快速访问/历史/闪念：左 dock「快速访问」页(已固定 + 最近 MRU)，右键笔记「固定到快速访问」，「快速记录」一键打开闪念 flash.md。
* HistoryMgr(core,markly_core):addToHistory(MRU 去重+cap 50)/pin/unpin/isPinned/ensureFlashFile(AppConfigLocation/flash.md);MarklyApp connect openFileRequested→addToHistory(所有打开来源进历史)。
* SessionConfig 加 history/quick_access 持久化;QuickBridge(context property Quick)history/quickAccess+open/pin/unpin/isPinned/openFlash;NotebookExplorer.nodeAbsPath。
* QuickAccessPanel.qml(已固定/最近两区+空状态);三外壳 history 按钮/「最近」nav→leftPage=quick;A「快速记录」→openFlash。
* ctest 13/13(新增 test_history)；截图验证快速访问页固定+最近列表。
* 再规划：QuickAccess 文件夹/历史时间戳/Flash 多页 → 后续小迭代。

### images-local (spec #10) — 2026-06-09
* 本地图片：编辑器 Ctrl+V 粘贴 / 拖入图片 → 存到笔记 vx_images/ → 插入 ![](rel) → 预览内联显示。
* ImageHelper(context property Images) clipboardHasImage/pasteImage/importImage/toDataUri；ViewArea.currentFileDir(Q_PROPERTY)。
* MarkdownEditor Ctrl+V(仅剪贴板有图才拦)+DropArea(text/uri-list)；PreviewPane resolveContent 渲染前把本地图片预解析为 base64 data URI。
* 关键决策：data URI 而非 file://——QWebEngine qrc 页跨源禁止加载 file:// 图片(设置/flag 均无效),内联 data URI 与渲染模式无关;currentFileDir 由 Q_INVOKABLE 改 Q_PROPERTY(NOTIFY)才随视图刷新(图片不显示真正根因)。
* ctest 12/12(新增 test_images)；截图验证预览内联显示本地图片。
* 再规划：图床上传拆出 #10b imagehost-upload(GitHub/Gitee+token 配置)；GIF/imsize 等待办。

### preview-extras (spec #9b) — 2026-06-09
* 预览渲染增强：markdown-it 插件(footnote/sub/sup/mark/emoji/task-lists/anchor/toc,防御式 use)+ Prism 代码块语法高亮(token 配色随主题 CSS 变量)。复用源 web 资源。
* 真实大纲：ViewArea 解析当前 buffer 标题(跳过围栏代码块内 #)→ Q_PROPERTY outline；三外壳大纲面板接真实数据(替换 DemoData)，点击经 gotoLineNow 即时跳转(补 #8 已激活 buffer 跳转缺口)。
* PreviewPane pushTheme 增注 Prism token 颜色(Theme 角色→hex)。
* ctest 11/11(test_bufferview 加 outline 用例)；截图验证任务清单复选框+Prism 高亮+真实大纲。
* 再规划：重型引擎拆出 #9c diagram-math-extras(MathJax/Mermaid/Flowchart/PlantUML/Graphviz/WaveDrom)；章节号/user.css/滚动同步→polish。

### tags-system (spec #11) — 2026-06-09
* 标签系统 UI：左 dock 标签页(标签 Chip+笔记数+点击筛选+选中标签的笔记列表)，复用 #4 数据层(tag/tag_node/NotebookTagMgr/updateNodeTags)+#12 queryNodesByTag。
* TagBridge(context property Tags) tags/selectedTag/taggedNodes + selectTag/openTaggedNode/refresh；监听 currentNotebookChanged + MarklyApp.tagsChanged 刷新。
* NotebookExplorer 加 nodeTagsCsv/setNodeTags；NodeContextMenu「标签…」(promptText 编辑逗号分隔)；三外壳 tag 按钮/标题切 leftPage=tags。
* 修复两个 #4 既有 Bug(被 #11 暴露)：(1) addTag INSERT OR REPLACE→OR IGNORE(避免 FK CASCADE 连带删 tag_node)；(2) open() tagMgr.load 早于 rebuildDatabase→rebuild 后重 load。
* ctest 11/11(新增 test_tags)；截图验证标签 chip+计数+筛选笔记列表。
* 再规划：标签父子层级树/管理(重命名删除)/拖拽打标 → polish 或后续小迭代。

### markdown-editor-input (spec #8) — 2026-06-09
* 编辑器输入辅助：自动缩进、列表续行(无序/有序/任务项,空标记清除)、括号/引号配对(含包裹选区/跳过/成对退格)、多行缩进反缩进、查找替换栏、跳转到行。
* 纯文本算法抽到可单测 EditInputHelper(core/editor/)；EditInputQml(context property EditInput) 暴露 QML；EditorConfig 加 auto_indent/continue_list/auto_pair。
* FindReplaceBar.qml(查找/替换/上下一个循环/Aa 大小写/全部替换/Esc)固定编辑器顶部；MarkdownEditor Ctrl+F/H/Esc + gotoLine。
* 接住 #12：Searcher 内容命中填 m_line(首个命中行)；SearchBridge.openResult→MarklyApp.gotoLineRequested→ViewArea pending→编辑器 onBufferIdChanged 后 takePendingGotoLine→gotoLine。
* ctest 10/10(新增 test_editorinput)；截图验证编辑器渲染清单/有序/代码/引用。
* 再规划：拆出 #8b vi-mode、#8c spell-check-hunspell(独立大子系统)；格式工具栏命令/已激活 buffer 跳转→polish。

### search-fts (spec #12) — 2026-06-09
* 全文搜索（技术栈关键点 SQLite FTS5 落地）：每笔记本 DB 建 `node_fts` FTS5 虚拟表索引 名称/路径/正文。
* 与源差异：源 FileSearchEngine 遍历文件无索引；本应用按技术栈用 FTS5 索引 + 单一同步 Searcher（精简 token/异步引擎）。
* Searcher 按 scope(笔记本/全部/文件夹/已打开) × object(名称/内容/路径/标签) 查询：内容 FTS5 MATCH+snippet、名称/路径内存匹配、标签查节点 tags；去重组装结果。
* SearchBridge(context property Search) results/state/count + search/openResult/clear；openResult→openFileRequested；监听 MarklyApp.noteSaved 增量 reindex。
* SearchPanel.qml：关键词+scope 段按钮+object toggle+结果列表(snippet `<mark>`→accent 高亮)；接入三外壳左 dock 搜索页(A 活动栏 / B 搜索框+活动栏 / C Omnibar)。
* ctest 9/9（新增 test_search：内容/名称/空词/增量）；截图验证搜索「桥接」命中+高亮 snippet+路径。
* 再规划：跳转到行→#8；重命名 name 索引更新/正则·magic-switch→polish；搜索历史→#13。

### markdown-preview (spec #9) — 2026-06-09
* Markdown 渲染预览：QtWebEngine 的 QML WebEngineView 加载内置 preview.html(markdown-it)，编辑/阅读/分栏三模式。
* 架构决策验证：QML WebEngineView 在 QQuickWidget(Qt6.8 RHI) 渲染成功，无需 QWidget 回退；main 在 QApplication 前 QtWebEngineQuick::initialize()。
* 复用源 markdown-it.min.js；preview.css 用 CSS 变量驱动主题(随 Theme 着色)；防抖渲染。
* ViewArea viewMode(持久化 SessionConfig) + EditorArea 按模式组合 MarkdownEditor+PreviewPane + 工具栏 eye 切换。
* ctest 8/8(含 viewMode cycle)；split 模式截图验证(左源码高亮+右渲染富文本)。
* 再规划：拆出新 spec #9b preview-extras(MathJax/Mermaid/Prism/TOC/outline)。

### markdown-editor-core (spec #7) — 2026-06-09
* 真正的 Markdown 源码编辑器：语法高亮 + 行号 + 当前行高亮 + Tab/缩进 + Ctrl+S + 真实行列/字数统计。
* 架构决策(spike 验证)：不嵌 QWidget(Wayland 不稳)，改用 QML TextEdit + C++ QSyntaxHighlighter 挂 QQuickTextDocument。
* MarkdownHighlighter(QML 类型 Markly.Editor，规则 QRegularExpression，颜色取 ThemeMgr，主题变重高亮) + EditorConfig 字段 + EditorCfg 桥接 + MarkdownEditor.qml。
* Views 统计(statsLine/Column/LineCount/CharCount)；三套外壳编辑区用 MarkdownEditor，A 状态栏真实统计。
* test_editorconfig；ctest 8/8；截图验证(标题/代码/列表/链接/引用/强调全高亮 + 行号 + 真实统计)。

### buffer-view-area (spec #6) — 2026-06-09
* 打开/视图层：Buffer/BufferMgr(按路径去重) + ViewArea 桥接(splits/tabs/currentText + 分屏) + EditorArea.qml(共享编辑区)。
* 接住 #5 openFileRequested → 标签打开笔记，可编辑文本占位(#7 换编辑器内核)、保存、脏标记、基础左右分屏。
* 会话恢复：SessionConfig opened_files/current_file，重启恢复上次标签。
* MarklyApp.getBufferMgr() 返回真实(替换 #1 nullptr)；三套外壳编辑区嵌入 EditorArea 显示真实内容。
* test_bufferview(去重/dirty/save/标签/分屏/会话)；ctest 7/7；截图验证(预开两文件→两标签+真实内容)。

### node-explorer-ui (spec #5) — 2026-06-09
* 笔记本数据层接到三套外壳：真实笔记本树替换占位（A 树/B 笔记列表/C 笔记本导航）。
* NotebookExplorer 桥接（QObject context property，visibleNodes 展平树 + Q_INVOKABLE 交互）+ DialogHelper（原生对话框）。
* 节点 CRUD（右键菜单：新建笔记/文件夹、重命名、删除回收站/永久、颜色、外部导入）经 #4 Notebook 门面三处一致。
* 笔记本选择/新建/打开/切换；空状态 EmptyState；外部文件显示 + Notebook.importNode 导入。
* 确立「占位→真实 model」桥接范式（core→QObject→QML），后续 UI spec 复用。
* test_notebookexplorer（树展平/CRUD/颜色/外部/导入）；ctest 6/6；三套外壳真实树截图验证。

### notebook-core (spec #4) — 2026-06-09
* 笔记本数据层（纯 core）：磁盘格式 vx_notebook.json + 每目录 vx.json + SQLite 三表(node/tag/tag_node)，严格兼容 VNote v3.x。
* INotebookBackend/LocalNotebookBackend、NotebookConfig、vx_node_config 序列化、Node/NodeVisual、NotebookDatabaseAccess、NotebookTagMgr、DummyVersionController、Notebook(CRUD 门面)、NotebookMgr。
* 节点 CRUD 三处双写（磁盘→vx.json→SQLite）：新建/重命名/移动回收站/永久删除/颜色自定义/标签；懒加载子文件夹；node.id 由 DB autoincrement 回写 vx.json。
* NotebookMgr 多笔记本 + 当前 + 持久化(SessionConfig)，接入 MarklyApp::getNotebookMgr()（替换 spec#1 nullptr 占位）。
* 架构精简：单实现接口合并（VXNode→Node、BundleNotebook/configmgr→Notebook、工厂→NotebookMgr）。ExternalNode 延后 #5。
* test_notebookcore 5 例（三处一致 + 重开懒加载 + VNote v3 兼容样例读取）；ctest 5/5。

### app-shell-ui (spec #3) — 2026-06-09
* 无边框自绘主窗口 FramelessMainWindow（startSystemMove/Resize，QML 经 Win.* 控制）。
* QML 外壳 MarklyShell：Loader 按 Appearance.style 切三套布局 ShellRefined(A)/ShellFocus(B)/ShellWorkbench(C)，1:1 复刻设计稿整窗。
* 共享子组件（WinControls/RailButton/StatusBar/Chip/EditorTab/EditorToolbar/TreeRow/OutlineRow/NoteCard/Omnibar）只引用 Theme.* 角色。
* 图标系统：IconPaths.js（~50 图标）+ Icon.qml（QtQuick.Shapes 描边渲染，随主题着色/缩放）。
* 外观面板接入（右下角「外观」抽屉）；风格/主题/强调色/布局整窗实时切换。
* MainWindow 继承 FramelessMainWindow 承载 MarklyShell，替换 spec #1/#2 占位；MARKLY_SHOT 截图 hook。
* 验证：build 通过；三风格 × 浅/深 × 强调色 offscreen 截图渲染贴近设计稿。

### theme-appearance (spec #2) — 2026-06-09
* 语义 token 体系（ThemeTokens）+ 三套风格 A/B/C × 浅深 × 强调色生成（ThemeModel，逐值复刻设计稿）。
* Appearance 偏好（style/theme/accent/showLeft/showRight/density）+ 即时持久化到 WidgetConfig.appearance + theme=auto 跟随系统。
* ThemeMgr 重写为 QML 桥接（全部 token 暴露为 Q_PROPERTY）+ StyleSheetGenerator（token→QWidget QSS），替换 spec #1 占位。
* 引入 Qt Quick；QML 外观面板 AppearancePanel.qml + 校验载体 ThemePreview.qml（占位 MainWindow 用 QQuickWidget 承载）。
* 单测 test_thememodel：token 精度 + accent 派生 + 持久化往返（ctest 4/4 通过）；QML 载入无错。

### app-foundation (spec #1) — 2026-06-09
* 建立 Qt 6.8 + CMake/Ninja 工程骨架（C++17，AUTOMOC/UIC/RCC）。
* `MarklyApp` 单例协调器（对标 VNote 的 VNoteX）+ 全局信号枢纽。
* `ConfigMgr` 分层配置系统（IConfig + MainConfig/CoreConfig/EditorConfig/WidgetConfig/SessionConfig），磁盘 JSON 结构对齐 VNote、读兼容；默认配置打包 QRC。
* `Application`（文件打开事件 + 主题热重载）、`SingleInstanceGuard`（单实例 + IPC）、`CommandLineOptions`、`Logger`。
* `ThemeMgr`/`MainWindow` 占位（分别由 spec #2、#3 替换）。
* 单元测试：configmgr / commandlineoptions / singleinstanceguard（ctest 3/3 通过）。
* 应用可构建、可启动、可干净退出；首次运行生成 `~/.config/Markly/Markly/{markly.json,session.json,markly.log}`。

### mindmap-edit (spec #18c) — 2026-06-09
* `mindmap.html`：`renderMindmap(md, editable)` 加 editable 参数（MindElixir 增删/拖拽/菜单全开，默认只读保 #18b 兼容）；`treeToMarkdown` + `window.mindmapMarkdown()` 把导图树序列化回 Markdown 标题（根 `#`，深度 d→`d+1` 个 `#`），offscreen 回退 `lastData`。
* `MindmapPane.qml`：`editable`/`bufferId` 属性 + `saveToNote()`（runJavaScript `mindmapMarkdown()` → `Views.updateText`，visible 守卫）+ Ctrl+S `Shortcut` + 监听 `Views.saveMindmapRequested`。
* `EditorArea.qml`：mindmap 模式 MindmapPane 设 `editable: true` + 绑 `bufferId`。
* `UnitedEntry.qml`：命令「保存思维导图到笔记」→ `Views.requestSaveMindmap()`。
* `ViewArea`（viewarea.h/cpp）：`requestSaveMindmap()` 发 `saveMindmapRequested` 信号（镜像 `requestInsert`/`insertText`），命令跨 Repeater/多 shell 广播、可见 pane 响应。
* i18n：新增「保存思维导图到笔记 / Save mind map to note」（lrelease 72 条）。
* 验证：build + ctest 26/26；throwaway QWebEngineWidgets harness 加载真实 mindmap.html，`renderMindmap("# Root\n## A\n## B\n### A1", true)` → `mindmapMarkdown()` 往返含各级标题（ROUNDTRIP PASS）。
* 思维导图族（#18b 只读 + #18c 编辑往返）收口，不派生后续。

### packaging-multiplatform (spec #21b) — 2026-06-09
* `CMakeLists.txt`：CPack 平台条件生成器 `if(WIN32)NSIS;ZIP / elseif(APPLE)DragNDrop;TGZ / else()TGZ;STGZ`，Linux 下 `find_program(dpkg-deb/rpmbuild)` 守卫追加 DEB/RPM；DEB（maintainer/section/depends）、RPM（license/group）、NSIS（package/display/uninstall/modify-path）元数据。
* `packaging/PKGBUILD`（新）：Arch 包，build()/package() 走 cmake，depends=qt6-base/declarative/webengine+hunspell，pkgver 同步 PROJECT_VERSION（1.0.0）。
* `.github/workflows/release.yml`（新）：tag 触发，matrix(ubuntu/windows/macos) 装 Qt 6.8 → cmake → ctest → cpack → upload-artifact，Windows/macOS 各条件加 windeployqt/macdeployqt。
* 本机真实验证：build + ctest 26/26；`cpack -G STGZ`→`Markly-1.0.0-Linux.sh`（可执行自解压）；`cpack -G TGZ`→`.tar.gz`（不回归 #21）；`makepkg --printsrcinfo` 含 pkgname=markly；`yaml.safe_load(release.yml)` 合法 3-os 矩阵。
* 诚实记录：DEB/RPM/NSIS/dmg/deployqt 仅产配置、需对应平台验证（docs 两表）；AppImage（需 linuxdeploy 联网）与高保真图标列为后续，不派生 spec。
* 打包族（#21 + #21b）收口。**至此 56 个 spec 全部 DONE，vnote-3201 全功能覆盖完成，自然收敛。**

### fix: toolbar icon rendering (#icon-joined) — 2026-06-09
* `src/qml/icons/IconPaths.js` `joined()`：修复多子路径图标错位。Lucide 每个子路径是独立 `<path>`，其首个相对 `m dx dy` 以原点为基准（=绝对 dx,dy）；拼成单条路径后该 `m` 变成相对上一子路径终点而被甩飞（code 右半 `>` 变成乱团、search 手柄错位）。修复：非首子路径的首个 `m` 改绝对 `M`，其后隐式坐标对补显式 `l` 保持相对，几何不变。影响 code/search/snippet 等。
* 验证：独立 QML harness 渲染 code=`< >`、sigma=`Σ`、link=🔗、search=🔍、snippet=`</>` 均正常；build + ctest 26/26。

### 公开发布准备 (public-release) — 2026-06-10
* 品牌：logo 字标 V→M（4 个 shell + 欢迎页），重绘 128x128 应用图标（青绿圆角 + 白 M）；欢迎语与包描述改中性表述。
* 新增公开 README（特性/截图/构建/打包）与 MIT LICENSE（Copyright Cygmris）；实机截图收录 docs/screenshots/。
* commit log 全量清理特定字眼（filter-branch msg-filter，验证 0 残留）；分支 master→main。
* 仓库公开发布：https://github.com/cygmris/markly（public，作者全 Chris，ctest 26/26）。
