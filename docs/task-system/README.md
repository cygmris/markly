# Markly — 任务系统（task-system spec #16）

外部命令任务：用户定义任务（JSON），运行时展开任务变量（当前笔记/笔记本上下文），经 `QProcess` 执行并显示输出。`MarklyApp::getTaskMgr()` 此前为 nullptr 占位，本 spec 替换为真实 `TaskMgr`。

## 组件

- **TaskMgr**（`core/task/`，markly_core）：
  - 任务 `{name, command, args[], shell}` 存 `<AppConfigLocation>/tasks/<name>.json`。`load()` 空目录则 `seedDefaults`（「字数统计」`wc -m "${bufferPath}"`）。
  - `expand(text, ctx)`：替换 `${bufferPath}`/`${bufferName}`/`${bufferBaseName}`/`${bufferDir}`/`${notebookFolder}`/`${cwd}`，未知保留、无值空。
  - `run(name, ctx, &exit, timeout=10000)`：展开 → `QProcess`（`shell` 经 `sh -c`，否则 program+args；工作目录 = `notebookFolder` 或 `bufferDir`；合并 stdout/stderr）；超时 kill；`FailedToStart` 返回错误串不崩；返回输出 + 写出退出码。
- **MarklyApp**：持有真实 `TaskMgr`（`getTaskMgr()`），`initLoad` 调 `load()`。
- **TaskBridge**（`widgets/task/`，context property `Tasks`，markly_core）：`Q_PROPERTY list`（`[{name}]`）；`Q_INVOKABLE run(name, bufferDir, bufferName)` 构造 `TaskContext`（bufferPath + 当前笔记本 rootPath）→ `TaskMgr.run` → 返回 `"[退出码 N]\n" + output`。
- **工具栏「任务」菜单**（`cmd` 图标）：`Instantiator` 列出 `Tasks.list`，点击 `Tasks.run(name, Views.currentFileDir, Views.currentFileName)` → `Dialogs.notify` 显示输出；空列表显禁用「（无任务）」。

## 变量（对齐 vnote 命名子集）

`${bufferPath}`（笔记绝对路径）、`${bufferName}`（文件名）、`${bufferBaseName}`（去扩展名）、`${bufferDir}`（目录）、`${notebookFolder}`（笔记本根）、`${cwd}`（= 笔记本根或笔记目录）。

## 验证

- ctest 16/16（新增 `test_task`）：
  - `expand`：`${bufferBaseName} ${notebookFolder} ${unknown}` → `note /nb ${unknown}`（展开 + 未知保留）。
  - `run`：`echo ${bufferBaseName}`（真实 QProcess）→ 输出含展开值、退出码 0。
  - `run` 命令缺失 → 非崩、有错误串。
  - `load`/seedDefaults：load 后任务非空。

## 后续（再规划）

- 异步/流式任务输出 + 专用输出面板（替代 notify）、任务编辑 UI、任务快捷键（绑定归 #17/#20）、更多变量（`${exeFile}`/`${appFolder}`）→ 后续小迭代。
