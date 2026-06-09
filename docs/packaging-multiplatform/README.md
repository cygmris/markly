# packaging-multiplatform (#21b) — 多平台打包

在 #21（Linux TGZ + 桌面集成 + smoke 测试）基础上加 **多平台 CPack 生成器** + **Arch PKGBUILD**
+ **GitHub Actions 发布矩阵**。打包族（#21 + #21b）至此收口。

## 环境诚实约束（必读）

本工程在 **Arch Linux** 上开发，本机仅 `makepkg` 可用，无 `dpkg-deb`/`rpmbuild`/`windeployqt`/
`macdeployqt`。因此严格区分「**本机真实验证**」与「**仅产配置、需对应平台才能验证**」——
遵四铁律「环境受限功能：可编译/可解析的真实配置 + 守卫 + 诚实记录，不伪造验证」。

### 本机已真实验证 ✅

| 产物 | 验证命令 | 结果 |
|------|----------|------|
| TGZ | `cd build && cmake .. && cpack -G TGZ -B _packages` | `Markly-1.0.0-Linux.tar.gz`（不回归 #21） |
| STGZ 自解压 | `cpack -G STGZ -B _packages` | `Markly-1.0.0-Linux.sh`（13MB，可执行 POSIX shell） |
| Arch PKGBUILD | `cd packaging && makepkg --printsrcinfo` | `pkgname = markly` / `pkgver = 1.0.0` |
| CI YAML | `python3 -c "import yaml; yaml.safe_load(open('.github/workflows/release.yml'))"` | 合法，矩阵 `[ubuntu, windows, macos]` |
| 构建 + 测试 | `ctest`（build 目录） | 26/26 全绿 |

### 仅产配置，不可在本 Linux 验证 ❌

| 产物 | 需要 | 说明 |
|------|------|------|
| DEB | `dpkg-deb` | CMake 内 `find_program` 守卫——有 dpkg-deb 时 `CPACK_GENERATOR` 自动追加 `DEB`，元数据（maintainer/section/depends）已配。 |
| RPM | `rpmbuild` | 同上守卫追加 `RPM`，license/group 已配。 |
| NSIS（Windows 安装器）/ ZIP | Windows | `if(WIN32)` 分支设 `NSIS;ZIP` + `CPACK_NSIS_*`。 |
| DragNDrop（macOS dmg）| macOS | `elseif(APPLE)` 分支设 `DragNDrop;TGZ`。 |
| windeployqt / macdeployqt | 对应平台 CI | release.yml 内条件步骤（`if: runner.os == ...`）。 |

## 各平台手动出包命令

```bash
# Linux（本机）
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build
cd build && cpack -G "TGZ;STGZ"          # 有 dpkg-deb/rpmbuild 时自动含 DEB/RPM

# Arch
cd packaging && makepkg -si              # 联网装 qt6 依赖后本地安装

# Windows（在 Windows + Qt 环境）
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release
windeployqt build/src/Release/markly.exe
cd build && cpack -G "NSIS;ZIP"

# macOS（在 macOS + Qt 环境）
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build
macdeployqt build/src/markly.app
cd build && cpack -G "DragNDrop;TGZ"
```

## CI（`.github/workflows/release.yml`）

推送 `v*` tag 触发：matrix(ubuntu/windows/macos) 各 job 装 Qt 6.8 → cmake 配置/构建 → ctest →
`cpack` → `upload-artifact`；Windows/macOS 各加 deployqt 条件步骤。**真实运行需推到 GitHub**，
本机仅验 YAML 合法性。

## 涉及文件

- `CMakeLists.txt` — CPack 平台条件生成器（`if(WIN32)/elseif(APPLE)/else()`）+ DEB/RPM/NSIS 元数据 + dpkg-deb/rpmbuild `find_program` 守卫。
- `packaging/PKGBUILD` — Arch 包（build/package 走 cmake；pkgver 手动同步 `PROJECT_VERSION`）。
- `.github/workflows/release.yml` — 多平台发布矩阵脚手架。

## 限制 / 后续（不派生 spec）

- **正式图标**：复用 #21 占位 `packaging/markly.png`；高保真图标设计为后续设计任务，非本 spec。
- **AppImage**：需 `linuxdeploy`/`appimagetool`（联网下载，本机无），为避免产出不可验证的死配置，
  本 spec **不**加 AppImage 配置；后续可选。
- 打包族（#21 + #21b）收口，**不派生后续 spec**。
