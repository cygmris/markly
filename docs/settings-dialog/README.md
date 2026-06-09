# Markly — 设置对话框（settings-dialog spec #19）

可搜索设置对话框，把已有配置（编辑器 EditorConfig、外观 Appearance）暴露为分类、可搜索、即时持久化的 UI。**作为统一容器**：后续 spec（Vi #8b / 拼写 #8c / 图床 #10b / 快捷键 #17·#20）落地时各自把设置项追加进本对话框。

## 组件

- **EditorCfgQml 可写**（#19）：`fontSize`/`tabWidth`/`expandTab`/`lineNumber`/`highlightCurrentLine`/`autoIndent`/`continueList`/`autoPair` 加 `WRITE` setter → 调既有 `EditorConfig` setter + `emit changed`（编辑器/预览绑定即时刷新）。fontSize clamp 8–32、tabWidth 1–8。
- **SettingsDialog.qml**：覆盖层 modal（半透明底 + 居中卡片）。顶部 标题 + 搜索框 + ✕；左 分类列表（编辑器/外观/关于）；右 当前分类设置项；搜索非空时跨分类按标签过滤。
  - 设置项 = JS 数组（`{cat,label,type,get,set,...}`），`get/set` 闭包绑 `EditorCfg`/`Appearance`。
  - 控件：内联 `Toggle`/`Stepper`/`SegSelect`/`Swatches`/`about` 组件（Loader 按 type 装载）。
  - 编辑器：字号、Tab 宽度、空格展开 Tab、显示行号、高亮当前行、自动缩进、列表续行、括号自动配对。
  - 外观：界面风格(A/B/C)、主题(浅/深/跟随)、强调色、显示左/右栏 —— 绑 `Appearance`（#2 已可写）。
  - 关于：技术栈/版本只读文本。
- **入口**：三外壳 settings gear（A 底部 rail + B 底部 rail + C 标题栏）→ `settingsDialog.show()`；各外壳内嵌一份 `SettingsDialog`（仅活动外壳存在）+ `openSettings()`。

## 验证

- ctest 15/15（`test_editorconfig` 扩展：autoIndent/continueList/autoPair setter + toJson 键往返）。
- 截图 `/tmp/markly-shots/settings19.png`：设置对话框「编辑器」分类显示 字号(14)/Tab 宽度(4) stepper + 6 个 toggle；分类列表 + 搜索框。
- offscreen 退出 WebEngine segfault(139) 仍为 #9 既有现象。

## 统一容器（再规划）

设置对话框是后续所有设置项的容器。各 spec 落地时**把自己的设置项 push 进 SettingsDialog 的 items 模型**（新增分类或追加到现有分类）：
- Vi 模式开关/键位 → #8b。
- 拼写检查开关/词典 → #8c。
- 图床配置（GitHub/Gitee token/repo） → #10b。
- 快捷键映射 → #17 / #20。

## 后续待办

- 设置项「恢复默认」/按分类 reset；设置项分组小标题；键盘导航。
