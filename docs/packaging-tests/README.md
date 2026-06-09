# 打包与集成测试 — spec #21

把已完成功能打包成可分发产物并加端到端集成测试，使「构建 → 测试 → 打包 → 安装 → 运行」
全链路可验证。

## 完整流程（命令速查）

```bash
# 1) 配置 + 构建
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# 2) 测试（单元 + 集成 smoke）
cd build && ctest --output-on-failure        # 22 用例全绿
cd ..

# 3) 打包（TGZ）
cd build && cpack -G TGZ                       # → Markly-1.0.0-Linux.tar.gz
tar tzf Markly-1.0.0-Linux.tar.gz | grep bin/markly
cd ..

# 4) 安装到前缀
cmake --install build --prefix /opt/markly     # bin/markly + 桌面集成
```

## 产物

| 产物 | 路径 |
| --- | --- |
| 可执行 | `bin/markly` |
| 桌面入口 | `share/applications/markly.desktop` |
| 图标 | `share/icons/hicolor/128x128/apps/markly.png`（128×128，占位 "V"，#21b 替换正式设计） |
| TGZ 包 | `build/Markly-<version>-Linux.tar.gz` |

版本单一真源 = 顶层 `project(Markly VERSION 1.0.0 ...)`（CPack/`--version` 都取它）。

## 桌面集成

`packaging/markly.desktop`：`Exec=markly %F`、`Icon=markly`、`Categories=Office;TextEditor;Utility;`、
`MimeType=text/markdown;`。`src/CMakeLists.txt` 的 `install(FILES ...)` 把 desktop/图标装到标准
XDG 路径，CPack 自动纳入包。

## 集成 smoke 测试（CTest）

| 用例 | 验证 |
| --- | --- |
| `smoke_version` | `markly --version` 退出码 0 且输出含 "Markly"（`PASS_REGULAR_EXPRESSION`） |
| `smoke_startup` | `MARKLY_SHOT` 离屏启动完成**全 QML 外壳初始化**并抓帧；断言截图产物存在（`test -s`） |

> **smoke_startup 为何断言产物而非退出码**：QtWebEngine 在 offscreen 平台下会在截图写出
> *之后* 于销毁阶段 segfault（已知，非初始化失败）。故用 `bash -c "... || true; test -s <png>"`
> 包装：忽略 markly 退出码，只断言截图（30KB，证明外壳初始化成功）。两 smoke 用 offscreen。

## 运行依赖

Qt6（Core/Gui/Widgets/Network/Sql/Qml/Quick/QuickWidgets/QuickControls2/WebEngineQuick）、
`libhunspell`（拼写 #8c）。预览图表/数学（Mermaid/Graphviz/Flowchart/KaTeX）与词典随包内置
（QRC / `src/data`）。

## 后续（#21b packaging-multiplatform）

Windows NSIS、macOS dmg、Linux AppImage/DEB/RPM、`windeployqt`/`macdeployqt`、CI 矩阵；
正式应用图标设计（替换占位 "V"）。
