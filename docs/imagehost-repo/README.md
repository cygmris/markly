# 通用 Git 仓库图床 — spec #10d

把图片提交到用户本地维护的某个 Git 仓库克隆（`git add`+`commit`+`push`），返回该图片的 raw URL
替换本地链接。至此图床三类全覆盖：GitHub（#10b）/ Gitee（#10c）/ Git 仓库（#10d）。

## 组成

| 文件 | 职责 |
| --- | --- |
| `src/core/imagehost/repoimagehost.{h,cpp}` | `rawUrl`（纯）+ `commitImage`（真实 git）+ `push`（网络） |
| `src/widgets/images/imagehostbridge.{h,cpp}` | `type=="repo"` 分派 + `ImageHostCfg` 加 localRepo/rawBase/subDir |
| `src/qml/shell/SettingsDialog.qml` | 图床类型加「Git 仓库」+ 本地仓库/Raw 前缀/子目录字段 |

## RepoImageHost

- `rawUrl(rawBase, branch, path)` → `<rawBase 去尾斜杠>/<branch>/<path>`。
- `commitImage(repoDir, relPath, data, message)`：`QDir.mkpath` 建目录 → 写文件 →
  `git -C repoDir add relPath` → `git -C repoDir -c user.name=Markly -c user.email=markly@local
  commit -m message`（**自带 identity，无需全局 git 配置**）。15s 超时；git 不可用/非仓库 → false。
- `push(repoDir)`：`git -C repoDir push`（网络，最佳努力；失败不影响已 commit）。

## 上传分派

`ImageHostBridge.configured()`：`type=="repo"` 且 local_repo/raw_base 非空。`upload()`：
`rel = sub_dir ? sub_dir+"/"+basename : basename` → `commitImage(local_repo, rel, data, ...)` →
成功则 `push`（best-effort）→ `emit uploaded(localPath, rawUrl(raw_base, branch, rel))`。命令/替换
沿用 #10b（UnitedEntry「上传图片到图床」+ MarkdownEditor `onUploaded`）。

> 前提：`local_repo` 是用户已 clone 好的图片仓库目录；自动 clone/凭据管理留作 #10e。

## 验证

- **单元测试** `tests/test_imagehost.cpp`（扩展，真实 git，能力守卫 `QSKIP`）：
  - `rawUrl` 正确（含尾斜杠去除）；
  - **真实 git**：`QTemporaryDir` `git init -b main` → `commitImage(dir,"img/a.png",bytes,"msg")` →
    true，`git log --oneline` 非空、`git ls-files` 含 `img/a.png`；非仓库目录 → false（不崩）。
  - **ctest 25/25 全绿**（test_imagehost 9 用例，含真实本地 git 提交）。
- 构建通过；SettingsDialog qmllint 无语法错误；offscreen 启动无错。
- push 走网络（离线不测）；本地 commit 真实验证。

## 后续小迭代（#10e）

首次自动 `clone`、push 异步 + 进度、多图床实例切换、删除远程图、SSH/凭据管理。
