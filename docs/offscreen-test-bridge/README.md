# offscreen-test-bridge — 后台无头自测桥

chrome-devtools 式「协议驱动」：offscreen 运行时监听本地 socket，直接调 app 方法 + 截图，
**不模拟鼠标、不抢焦点、不碰桌面**，彻底绕开 Wayland 鼠标授权/焦点问题。

## 用法
```bash
export MARKLY_TEST_SOCKET=/tmp/mk.sock
QT_QPA_PLATFORM=offscreen MARKLY_TEST_SOCKET=$MARKLY_TEST_SOCKET ./build/src/markly &
sleep 4
scripts/markly-test.sh dialog image      # 打开插入图片对话框
scripts/markly-test.sh wait 600
scripts/markly-test.sh shot /tmp/d.png    # → OK 798x720（含对话框浮层）
scripts/markly-test.sh insert "**x**"     # 光标处插入
scripts/markly-test.sh text               # → OK <base64(笔记全文)>
scripts/markly-test.sh quit
```

## 命令表
| 命令 | 作用 | 回复 |
|------|------|------|
| `dialog <kind>` | MarklyShell.showDialog（image/link/table/math/new-note/new-notebook/wordcount/export…） | OK |
| `content <kind>` | Views.requestContentDialog（工具栏路径） | OK |
| `viewmode <mode>` | edit/read/split/mindmap | OK |
| `insert <text>` | Views.requestInsert（光标处） | OK |
| `text` | 当前笔记全文 | OK <base64> |
| `shot <path>` | grabFramebuffer 存 PNG（含 Popup 浮层） | OK WxH |
| `wait <ms>` | 跑事件循环 ms（给动画/异步稳定） | OK |
| `quit` | 优雅退出 | OK |

## 实现
- `src/widgets/test/testbridge.{h,cpp}`：QLocalServer 行协议，env `MARKLY_TEST_SOCKET` 门控（普通运行零影响）。
- `MarklyShell.qml` 薄封装 `testViewMode/testInsert/testContent/testNoteText`，showDialog 复用。
- 截图用 `QQuickWidget::grabFramebuffer()`——能抓到 Overlay 里的 Popup（QQuickWindow::grabFramebuffer 抓不到）。

## 关键点
- **offscreen + QQuickWidget::grabFramebuffer 可抓对话框浮层**（这是之前 MARKLY_SHOT 抓不到 Popup 的解法）。
- C++ 调 QML 函数：`root` 须含 QQuickItem 完整类型（include <QQuickItem>），否则 invokeMethod 模板 SFINAE 失败。
