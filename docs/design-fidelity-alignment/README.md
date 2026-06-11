# design-fidelity-alignment — 设计保真度对齐（第一批：kit + 4 编辑器对话框）

把"按精神自写"的对话框严格对齐 Claude Design 交付稿（refined-kit / refined-editor-dialogs.jsx）。

## 完成（tasks 1-6，均 env 钩子截图比对设计稿）
- **kit 组件补齐**（components/）：MklField（label+可选+hint）、MklTextArea、MklSegmented（分段，可带 icon）、
  MklToggle、MklCheckbox、MklStepper（数字步进）、MklDropzone（虚线拖拽区）；MklTextField 加 prefix；
  MklButton 加 iconName；MklModal 加 dlgWidth。token 全走 Theme（borderStrong/link/selection 等已具备）。
- **截图验证钩子**：`MARKLY_SHOT_DIALOG=<kind>` 启动后自动打开对话框（独立于 MARKLY_SHOT，不退出），
  外部 `import -window` 截图比对设计稿。kind: image/link/table/math/wordcount/new-note/new-notebook/export。
- **插入图片**（500）：来源分段（本地/网络/剪贴板）+ 拖拽区/URL + 标题/宽度两列 + 图床开关。
- **插入链接**（460）：显示文本/链接地址(prefix)/提示标题 + 预览行（[文本] link 色）。
- **插入表格**（480）：行列 Stepper + 对齐分段 + 包含表头 Checkbox + 网格预览（表头行高亮）+ 对齐标记生成。
- **插入数学公式**（500，新增）：类型分段 + LaTeX TextArea + 12 符号面板 + 预览。工具栏 sigma 接此对话框。

## 关键教训（本批踩坑）
1. **font.pixelSize 必须 int**：写 13.5 → "Invalid property assignment: int expected" → 组件 unavailable →
   级联使 AppDialogs/ShellRefined 整个加载失败（白屏）。这正是前两期对话框样式没对齐却没暴露的原因
   （无法点开就没发现）。已全部改整数。
2. **AUTORCC 陈旧**：编辑 .qml 后 `cmake --build` 常不重跑 rcc，运行的是旧 QML。**每次改 QML 必须
   `rm build/src/.qt/rcc/qrc_markly_qml.cpp` 再 build**。本批大量"对话框没打开"假象皆因此。
3. **console.log 被日志器吞**：自定义 message handler 只记 Warning+，qDebug(console.log) 不出现；
   调试 QML 用 qWarning 或看 invokeMethod 返回值。
4. **color 不能拼进字符串**：RichText `'<span style="color:'+Theme.faint+'">'` 会坏掉绑定/布局
   → 改用独立 Text 分段着色。

## 待续（tasks 7-8，下一批）
- 导出对话框（范围/格式卡/选项/进度条）替换简单导出。
- 新建笔记/文件夹/笔记本对齐 refined-dialogs.jsx（模板/位置/图片附件目录/版本控制）。
