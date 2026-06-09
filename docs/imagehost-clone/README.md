# Git 仓库图床首次自动 clone — spec #10e

为 Git 仓库图床（#10d）补**首次自动 clone**：配置了 `clone_url` 但本地 `local_repo` 还不是 git
仓库时，首次上传前 `git clone`。clone 命令构造 + 「是否已是仓库」判定为纯函数/真实 git（可单测）；
clone 本身走网络（最佳努力）。**图床族（#10 本地 + #10b GitHub + #10c Gitee + #10d repo + #10e
clone）收口。**

## 组成

| 文件 | 改动 |
| --- | --- |
| `src/core/imagehost/repoimagehost.{h,cpp}` | `cloneArgs`（纯）/`isGitRepo`（文件系统）/`ensureCloned`（真实 git clone） |
| `src/widgets/images/imagehostbridge.cpp` | repo upload 前 `ensureCloned`；`ImageHostCfg.cloneUrl` |
| `src/qml/shell/SettingsDialog.qml` | 图床「克隆地址」字段 |

## RepoImageHost 扩展

- `cloneArgs(repoUrl, destDir)` → `["clone", repoUrl, destDir]`。
- `isGitRepo(dir)` → `<dir>/.git` 存在。
- `ensureCloned(repoUrl, destDir)`：`isGitRepo(destDir)` → 直接 true（**快路径，零网络**）；
  否则 repoUrl 空 → false；否则 `git clone`（60s 超时，git 守卫），成功 → true。

`ImageHostBridge.upload`（type==repo）在 commitImage 前调 `ensureCloned(clone_url, local_repo)`——
已是仓库则跳过（零开销），失败则 `emit failed`。

## 验证

- **单元测试** `tests/test_imagehost.cpp`（扩展，真实 git，能力守卫）：
  - `cloneArgs("https://x/r.git","/tmp/r")` == `{clone, https://x/r.git, /tmp/r}`；
  - `isGitRepo`：临时目录无 `.git` → false，`git init` 后 → true；
  - `ensureCloned` 快路径：`git init` 后 `ensureCloned("", dir)` → true（已是仓库，无需 URL/网络）；
    非仓库 + 空 URL → false。
  - **ctest 26/26 全绿**（test_imagehost 10 用例）。
- 构建通过；SettingsDialog qmllint 无语法错误；offscreen 启动无错。
- 真实 `git clone` 走网络（离线不测，诚实记录）；已是仓库快路径真实验证。

## 图床族收口

GitHub（#10b）/ Gitee（#10c）/ Git 仓库（#10d 本地 commit + #10e 自动 clone）三类全覆盖。多图床
实例切换、删除远程图、SSH/凭据管理可在配置层按需扩展，**不另派生 spec**。
