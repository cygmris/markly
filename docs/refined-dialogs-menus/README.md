# refined-dialogs-menus — 设计系统菜单与对话框（第一期）

基于 2026-06-10 Claude Design 交付（refined-kit/menus/dialogs + chat2），把高频按钮的点击体验
从原生 QInputDialog 升级为石墨青设计语言。

## 落地内容
- **UI kit**（`components/`）：MklModal（460/r14/图标头/footer 按钮区）、MklMenu+Item/Sep/Label
  （r11/danger 红/checked/shortcut/sub）、MklTextField（38h 焦点描边）、MklButton（primary/ghost/danger）、Toast（1.8s 自隐）。
- **「+ 新建」菜单**：新建笔记（accent+sub）/新建文件夹/新建笔记本…，树面板 + 按钮锚定弹出。
- **对话框**（`AppDialogs.qml`）：新建笔记（.md 后缀）/新建文件夹/新建笔记本（浏览根目录+名称）/重命名/删除确认（danger 主按钮+不可恢复提示），接 Explorer 桥。
- **右键菜单**重写：打开(accent)/新建类/重命名/标签/固定/颜色/复制路径(新增 Dialogs.copyText 桥)/删除到回收站(走确认对话框)/永久删除，danger 红样式。
- **保存 toast**：Ctrl+S 后弹「已保存」（实测截图验证）。
- **外观按钮避让**：状态栏右侧 rightMargin 110（设计 chat2 修复方案）。

## 后续（不在本期）
插入图片/链接/表格/公式对话框、导出进度、属性、字数统计、管理笔记本、排序/导入/编辑器更多菜单、
闪念快速记录、设置 11 分类页、B/C shell 复用 —— 见 .claude-design refined-* 设计稿，按需开后续 spec。

## 验证
构建 + ctest 26/26；offscreen/实机启动无 QML 报错；toast 实机截图验证；菜单/对话框点击路径待手工复验
（本机当日指针注入失效）。
