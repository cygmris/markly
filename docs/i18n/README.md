# Markly — 国际化（i18n spec #20）

i18n 基础设施：Qt 翻译加载链路（`QTranslator` + `.ts`/`.qm`）、语言偏好、设置内语言切换（即时重翻译），代表性 UI 字符串纳入 `qsTr()`（英文翻译示范）。全量字符串覆盖是**持续任务**。

## 再规划：系统托盘 + 更新检查拆 #20b

原 #20 的系统托盘/更新检查/文件关联/NavigationMode 依赖桌面托盘/网络、离线难验证，拆出 **#20b tray-update**。本 spec 只做 i18n。

## 组件

- **WidgetConfig.language**（`auto`/`zh_CN`/`en_US`，默认 auto）持久化（markly.json `widget.language`）。
- **MarklyApp.applyLanguage(lang)**：解析 `auto`→`QLocale::system().name()`；移除旧 `QTranslator`；`zh*`→源语言（不加载，回退中文源串）；否则 `load(":/i18n/markly_<catalog>.qm")` 成功则 `installTranslator`。
- **启动**：`main` 在构建任何 UI 前调 `applyLanguage(WidgetConfig.language)`。
- **LocaleBridge**（context property `Locale`，markly_core）：`Q_PROPERTY language`；`setLanguage(lang)` → 写配置 + `applyLanguage` + `engine->retranslate()`（QML `qsTr()` 绑定即时重算）+ emit changed。MainWindow 用 `m_quick->engine()` 构造。
- **qsTr 子集**：`SettingsDialog`（标题/分类/搜索/各项 label/语言项）、`UnitedEntry`（placeholder/命令 label）用 `qsTr()`。分类用**稳定 key**（editor/appearance/about）匹配 + 翻译 label 显示，避免 retranslate 时 key 失配。
- **设置「语言」**：`langseg` 控件（字符串值 seg：跟随系统/中文/English）绑 `Locale`。
- **翻译**：`src/i18n/markly_en_US.ts`（context = QML 文件名）→ `lrelease` 生成 `markly_en_US.qm`（32 条），打包 `markly_core` QRC `:/i18n/`（便于 test 访问）。

## 验证

- ctest 17/17（新增 `test_locale`）：WidgetConfig.language 往返 + `QTranslator` 加载 `:/i18n/markly_en_US.qm` → `translate("SettingsDialog","设置")=="Settings"`、`translate("UnitedEntry","新建笔记")=="New note"`。
- 截图 `/tmp/markly-shots/i18n20.png`：`language=en_US` 启动 → 设置对话框**全英文**（Settings / Editor / Appearance / About / Font size / Tab width / Expand tab to spaces / Show line numbers / Auto indent…）。
- offscreen 退出 segfault(139) 仍为既有现象。

## 扩展全量翻译（持续任务）

1. 把更多 QML 字符串包 `qsTr()`、C++ 串包 `tr()`。
2. `lupdate src/qml/... -ts src/i18n/markly_en_US.ts` 扫描更新 `.ts`，翻译，`lrelease` 重生成 `.qm`。
3. 新增语言：复制 `.ts` 为 `markly_ja_JP.ts` 等，翻译，加 QRC + applyLanguage 的 catalog 映射。

## 后续（再规划）

- **#20b tray-update**：系统托盘（QSystemTrayIcon）+ 菜单、更新检查、文件关联、NavigationMode。
- 全量字符串覆盖、日文翻译、`lupdate`/`lrelease` CMake 集成（Qt6LinguistTools）→ 持续任务。
