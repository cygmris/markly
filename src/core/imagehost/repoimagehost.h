#ifndef MARKLY_REPOIMAGEHOST_H
#define MARKLY_REPOIMAGEHOST_H

#include <QByteArray>
#include <QString>

namespace markly {
// Generic Git-repo image host (#10d): commit the image into a local clone via the real
// `git` CLI and return its raw URL. rawUrl is pure (testable); commitImage runs git.
class RepoImageHost {
public:
  // <rawBase>/<branch>/<path>, with a trailing slash trimmed from rawBase.
  static QString rawUrl(const QString &p_rawBase, const QString &p_branch, const QString &p_path);

  // Write data to <repoDir>/<relPath>, git add + commit (self-contained user identity).
  static bool commitImage(const QString &p_repoDir, const QString &p_relPath,
                          const QByteArray &p_data, const QString &p_message,
                          QString *p_err = nullptr);

  // git push (network, best-effort). Failure does not invalidate a prior commit.
  static bool push(const QString &p_repoDir, QString *p_err = nullptr);

  // First-run auto-clone (#10e). cloneArgs / isGitRepo are pure / filesystem (testable).
  static QStringList cloneArgs(const QString &p_repoUrl, const QString &p_destDir);
  static bool isGitRepo(const QString &p_dir);
  // True if destDir is already a repo (fast path, no network), or git clone succeeds.
  static bool ensureCloned(const QString &p_repoUrl, const QString &p_destDir,
                           QString *p_err = nullptr);
};
} // namespace markly

#endif // MARKLY_REPOIMAGEHOST_H
