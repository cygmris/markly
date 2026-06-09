# 拼写右键建议菜单 — spec #8d

承接 #8c（Hunspell 拼写检查 + 波浪下划线）：在编辑器对拼错单词右键弹出 Hunspell 建议
列表，点选替换；并可「添加到忽略列表」（本会话忽略，去红）。

## 组成

| 文件 | 改动 |
| --- | --- |
| `src/core/spell/spellchecker.{h,cpp}` | 会话忽略列表 `addIgnore`/`isIgnored`；`check()` 对忽略词返回 true |
| `src/widgets/spell/spellbridge.{h,cpp}` | QML `Spell` 桥：`enabled`/`misspelled`/`suggest`/`ignore` |
| `src/widgets/editors/markdownhighlighter.{h,cpp}` | `Q_INVOKABLE rehighlightNow()` |
| `src/qml/shell/MarkdownEditor.qml` | 右键 `MouseArea` + 拼写 `Menu`（建议 + 忽略）+ `Connections onIgnored` |
| `src/widgets/mainwindow.cpp` | 注册 `Spell` 上下文属性 |
| `src/i18n/markly_en_US.{ts,qm}` | 「添加到忽略列表」→ "Add to ignore list"（34 条） |

## 关键设计：忽略列表放共享 SpellChecker

会话忽略集合放进 **#8c 的共享 SpellChecker**（`m_ignore`，大小写不敏感），`check()` 对
忽略词直接返回 true。好处：**波浪下划线高亮器（走 `check()`）与建议菜单都自动尊重忽略**，
无需各维护一份。`SpellBridge` 因此是薄委托。

## 右键流程

1. `MouseArea`（仅 `RightButton`，叠在 `TextEdit` 上）`onPressed`：
   - `Spell.enabled()` 为假 → `mouse.accepted = false`（放行默认）。
   - `edit.positionAt(x,y)` 取字符位，按 `[A-Za-z]` 向左右扩展得单词 `[s,e)`。
   - `Spell.misspelled(word)` 为真 → 填 `spellSuggestions = Spell.suggest(word)`（上限 8），`spellMenu.popup()`；
     否则放行默认。
2. `Menu`：`Repeater(spellSuggestions)` → `MenuItem`，点选 `edit.remove(s,e)` + `edit.insert(s, 建议)`；
   分隔线；「添加到忽略列表」→ `Spell.ignore(word)`。
3. `Connections { target: Spell; onIgnored: mdHighlighter.rehighlightNow() }` → 忽略后去红。

## 验证

- 单元测试 `tests/test_spell.cpp`（核心新逻辑=忽略）：`check("helllo")` false →
  `addIgnore("helllo")` → `check("helllo")` true；`isIgnored("HELLLO")` true（大小写不敏感）。
  **ctest 18/18 全绿**（test_spell 4 用例）。
- 构建通过；应用 offscreen 启动无 QML 错误；`MarkdownEditor.qml` qmllint 无语法错误。
- SpellBridge 薄委托 + 建议来源由 #8c `suggest()` 单测背书；右键菜单/替换为编辑器交互
  （需已打开笔记本会话，offscreen 不截图），逻辑经构建期 QML 类型/语法检查。

## 后续

- 「添加到自定义词典」持久化（写 `<config>/dicts/personal.dic` 或 Hunspell add），
  多语言词典切换 → 后续小迭代。
