# Markly — 本地图片（images-local spec #10）

编辑器中粘贴/拖入图片 → 保存到笔记的 `vx_images/` → 插入 markdown 图片语法 → 预览内联显示。图床上传拆 #10b。

## 组件

- **ImageHelper**（`widgets/images/`，context property `Images`，markly_core）：
  - `clipboardHasImage()`：剪贴板是否含图像。
  - `pasteImage(noteDir)`：剪贴板图像存为 PNG 到 `<noteDir>/vx_images/img_<msec>.png`，返回相对路径。
  - `importImage(srcPathOrUrl, noteDir)`：复制图片文件（path 或 file:// url）进 `vx_images`，校验扩展（png/jpg/jpeg/gif/bmp/webp），返回相对路径。
  - `toDataUri(src, baseDir)`：把本地图片读成 `data:image/...;base64,...` 供预览内联。
- **ViewArea.currentFileDir**（Q_PROPERTY，NOTIFY changed）：当前激活笔记的目录。
- **MarkdownEditor**：Ctrl+V 仅当 `clipboardHasImage` 时拦截 → `pasteImage` → 插入 `![](rel)`（否则普通粘贴）；`DropArea`（text/uri-list）拖入图片 → `importImage` 逐个插入。
- **PreviewPane**：`baseDir` 属性；`pushContent` 前用 `resolveContent()` 把 markdown 里的本地图片路径**预解析为 data URI**（正则 `!\[..\]\(path\)` → `Images.toDataUri`），再 `mdRender`。

## 关键决策：data URI 而非 file://

QWebEngine 的预览页从 `qrc:` 加载，其 origin 是 qrc；浏览器**跨源策略禁止 qrc 页加载 `file://` 图片**（试过 `localContentCanAccessFileUrls` 与 `--allow-file-access-from-files` 均无效，offscreen 软件渲染下尤甚）。改为**在渲染前把本地图片内联为 base64 data URI**（C++ 读盘 + QML 同步预替换）——与 WebEngine 文件访问策略、渲染模式都无关，可靠且可截图验证。

`currentFileDir` 最初写成 `Q_INVOKABLE`，QML `baseDir: Views.currentFileDir()` 只在绑定建立时求值一次（彼时为空）且不随视图更新；改为 **`Q_PROPERTY` + NOTIFY changed** 后随笔记切换刷新——这是图片不显示的真正根因。

## 验证

- ctest 12/12（新增 `test_images`：importImage 复制 + 扩展校验 + file:// url；clipboard/paste 依赖显示不单测）。
- 截图 `/tmp/markly-shots/images10f.png`：read 模式预览内联显示 `vx_images/demo.png`（渐变 PNG）；data URI 解析生效。
- offscreen 退出 WebEngine segfault(139) 仍为 #9 既有现象。

## 后续（再规划）

- **#10b imagehost-upload**：GitHub/Gitee 图床上传、图床配置（token/repo/owner）、上传后插入远程 URL。
- 粘贴时询问「本地/图床」、图片右键复制/另存、GIF 录制、图片缩放语法（imsize）→ 待办。
