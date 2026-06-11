# editor-content-dialogs — 编辑器内容对话框（设计第二期）

基于 refined-editor-dialogs.jsx / EditorMoreMenu 设计：

- **插入链接**：文字+URL → `[文字](URL)`（空文字回退 URL）。
- **插入图片**：路径/URL（含「浏览…」本地选图，新增 `Dialogs.chooseFile` 桥）+ 可选替代文字 → `![alt](src)`。
- **插入表格**：行/列数（1..20 夹取，默认 3×3）→ 表头+分隔+空行骨架。
- **字数统计**：字符/非空白字符/行数（读 `Views.currentText`）。
- **编辑器更多菜单**（moreV）：字数统计…/查看思维导图/编辑模式。
- 工具栏 image/link/table/moreV 改走 `Views.requestContentDialog(kind)` →
  `contentDialogRequested` 信号 → AppDialogs/Shell Connections（仅 Refined 实例化生效，B/C 后续复用）。

验证：构建 + ctest 26/26；启动零 QML 报错；信号链路与第一期 toast 同构（指针注入当日不可用，按钮点击待手工复验）。
