# GitHub 图床上传 — spec #10b

把笔记里的本地图片上传到 GitHub 仓库（Contents API），用返回的公开 URL 替换 Markdown
里的本地链接。复用 #10 的图片基础。核心请求/响应构造为纯函数（可单测），异步上传用
`QNetworkAccessManager`。

## 组成

| 文件 | 职责 |
| --- | --- |
| `src/core/imagehost/githubimagehost.{h,cpp}` | GitHub Contents API：静态构造器 + 异步 `upload` |
| `src/core/widgetconfig.{h,cpp}` | `image_host { type, user, repo, token, branch }` 读写 |
| `src/widgets/images/imagehostbridge.{h,cpp}` | `ImageHost`（上传+信号）、`ImageHostCfg`（配置属性） |
| `src/qml/shell/SettingsDialog.qml` | 新增 `text` 输入类型 + 「图床」分类（类型/用户名/仓库/令牌/分支） |
| `src/qml/shell/UnitedEntry.qml` | 命令「上传图片到图床」 |
| `src/qml/shell/MarkdownEditor.qml` | `ImageHost.uploaded` → 文本里本地路径替换为远程 URL |

## GitHub Contents API

- 上传：`PUT https://api.github.com/repos/<user>/<repo>/contents/<path>`
  - 头：`Authorization: token <token>`、`Accept: application/vnd.github+json`、`Content-Type: application/json`
  - 体：`{ "message": "MARKLY_ADD: <path>", "content": "<base64>", "branch": "<branch>"? }`
  - 响应：`content.download_url` 即公开 URL。

静态纯函数（可单测）：`uploadUrl` / `uploadBody`（base64 + 可选 branch）/ `authHeader` /
`parseDownloadUrl`。异步 `upload` 成功 `emit done(true, url)`，失败 `done(false, error)`。

## 配置

存 `markly.json` 的 `widget.image_host`。`ImageHostBridge.configured()` 在 `type==github`
且 user/repo/token 非空时为 true。设置对话框「图床」分类用新增的 `text` 输入类型
（令牌字段 `echoMode: Password`）读写 `ImageHostCfg`。

> 安全性：token 以明文存 `markly.json`（与 vnote 一致）；请使用最小权限（仅目标仓库
> `contents` 写）的细粒度 PAT。

## 上传与替换

统一入口命令「上传图片到图床」（仅 `ImageHost.configured()` 时有效）：正则扫
`Views.currentText` 的 `![alt](path)`，去重收集本地（非 http/data）路径，逐个
`ImageHost.upload(path, Views.currentFileDir)`。编辑器 `Connections onUploaded(localPath, url)`
把文本里的 `localPath` 全部替换为 `url`。

## 验证

- **单元测试** `tests/test_imagehost.cpp`（GUILESS，不打真实网络）：
  - `uploadUrl("u","r","img/a.png")` == 期望串；
  - `uploadBody("p.png","hi","main")` → `content=="aGk="`、message 含 "p.png"、`branch=="main"`；
    branch 空时无 branch 键；
  - `authHeader("T")` == "token T"；
  - `parseDownloadUrl` 取 `content.download_url`、空对象 → ""；
  - `WidgetConfig.image_host` 往返。
  - **ctest 20/20 全绿**。
- 构建通过；SettingsDialog/UnitedEntry/MarkdownEditor qmllint 无语法错误；offscreen 启动无错。
- 真实网络上传需 token/仓库（离线不测）；请求构造/配置/解析由单测覆盖，上传链路经构建 + 信号接线。

## 后续小迭代（#10c imagehost-extra）

Gitee 图床、通用 Git 仓库图床、多图床配置管理、删除远程图、上传进度提示。
