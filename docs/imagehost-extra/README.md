# Gitee 图床 — spec #10c

在 #10b（GitHub 图床）基础上增加 **Gitee 图床**。复用 #10b 的图床框架（配置/桥/设置/上传
替换），新增 `GiteeImageHost`，`ImageHostBridge` 按 `image_host.type` 分派。

## 组成

| 文件 | 改动 |
| --- | --- |
| `src/core/imagehost/giteeimagehost.{h,cpp}` | Gitee Contents API：静态构造器 + 异步 POST |
| `src/widgets/images/imagehostbridge.{h,cpp}` | 加 `GiteeImageHost` 成员 + 按 type 分派；`configured()` 含 gitee |
| `src/qml/shell/SettingsDialog.qml` | 「图床类型」加 Gitee 选项（none/github/gitee） |

## Gitee vs GitHub（关键差异）

| | GitHub（#10b） | Gitee（#10c） |
| --- | --- | --- |
| 方法 | `PUT` | `POST` |
| 路径 | `api.github.com/repos/<u>/<r>/contents/<p>` | `gitee.com/api/v5/repos/<u>/<r>/contents/<p>` |
| 鉴权 | `Authorization: token <t>` 头 | `access_token` 在 JSON 体 |
| 体 | `{message, content, branch?}` | `{access_token, message, content, branch?}` |
| 响应 | `content.download_url` | `content.download_url`（相同，**复用** `GithubImageHost::parseDownloadUrl`） |

## 分派

`ImageHostBridge.configured()`：`type ∈ {github, gitee}` 且 user/repo/token 非空。
`upload()` 读字节后按 `type` 调 `m_host`（GitHub）或 `m_gitee`（Gitee），两者的 `done` 信号
经临时 connection 回链到 `uploaded`/`failed`（统一 `onDone` lambda）。上传命令与链接替换
沿用 #10b（UnitedEntry「上传图片到图床」+ MarkdownEditor `onUploaded`）。

## 验证

- **单元测试** `tests/test_imagehost.cpp`（扩展，GUILESS，不打真实网络）：
  - `GiteeImageHost::uploadUrl("u","r","a.png")` == `https://gitee.com/api/v5/repos/u/r/contents/a.png`；
  - `GiteeImageHost::uploadBody("p.png","hi","main","TOK")` → `access_token=="TOK"`、`content=="aGk="`、
    message 含 "p.png"、`branch=="main"`；branch 空时无 branch 键；
  - 响应解析复用 GitHub 的（已测）。
  - **ctest 22/22 全绿**（test_imagehost 7 用例）。
- 构建通过；SettingsDialog qmllint 无语法错误；offscreen 启动无错。

## 后续小迭代（#10d）

通用 Git 仓库图床（git clone/commit/push，QProcess）、多图床实例管理、删除远程图、上传进度。
