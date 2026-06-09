#include "repoimagehost.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>

using namespace markly;

namespace {
// Run `git -C <repoDir> <args>`; returns true on exit code 0. *err = stderr on failure.
bool runGit(const QString &repoDir, const QStringList &args, QString *err) {
  if (QStandardPaths::findExecutable(QStringLiteral("git")).isEmpty()) {
    if (err) *err = QStringLiteral("git not found");
    return false;
  }
  QProcess p;
  p.start(QStringLiteral("git"), QStringList{QStringLiteral("-C"), repoDir} + args);
  if (!p.waitForFinished(15000)) {
    if (err) *err = QStringLiteral("git timed out");
    return false;
  }
  if (p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0) {
    if (err) *err = QString::fromUtf8(p.readAllStandardError());
    return false;
  }
  return true;
}
} // namespace

QString RepoImageHost::rawUrl(const QString &p_rawBase, const QString &p_branch,
                              const QString &p_path) {
  QString base = p_rawBase;
  while (base.endsWith(QLatin1Char('/'))) {
    base.chop(1);
  }
  return QStringLiteral("%1/%2/%3").arg(base, p_branch, p_path);
}

bool RepoImageHost::commitImage(const QString &p_repoDir, const QString &p_relPath,
                                const QByteArray &p_data, const QString &p_message,
                                QString *p_err) {
  const QString abs = QDir(p_repoDir).filePath(p_relPath);
  if (!QDir().mkpath(QFileInfo(abs).absolutePath())) {
    if (p_err) *p_err = QStringLiteral("cannot create directory for %1").arg(p_relPath);
    return false;
  }
  QFile f(abs);
  if (!f.open(QIODevice::WriteOnly)) {
    if (p_err) *p_err = QStringLiteral("cannot write %1").arg(abs);
    return false;
  }
  f.write(p_data);
  f.close();

  if (!runGit(p_repoDir, {QStringLiteral("add"), p_relPath}, p_err)) {
    return false;
  }
  return runGit(p_repoDir,
                {QStringLiteral("-c"), QStringLiteral("user.name=Markly"),
                 QStringLiteral("-c"), QStringLiteral("user.email=markly@local"),
                 QStringLiteral("commit"), QStringLiteral("-m"), p_message},
                p_err);
}

bool RepoImageHost::push(const QString &p_repoDir, QString *p_err) {
  return runGit(p_repoDir, {QStringLiteral("push")}, p_err);
}

QStringList RepoImageHost::cloneArgs(const QString &p_repoUrl, const QString &p_destDir) {
  return {QStringLiteral("clone"), p_repoUrl, p_destDir};
}

bool RepoImageHost::isGitRepo(const QString &p_dir) {
  return QFileInfo::exists(p_dir + QStringLiteral("/.git"));
}

bool RepoImageHost::ensureCloned(const QString &p_repoUrl, const QString &p_destDir,
                                 QString *p_err) {
  if (isGitRepo(p_destDir)) {
    return true; // already a repo: fast path, no network
  }
  if (p_repoUrl.isEmpty()) {
    if (p_err) *p_err = QStringLiteral("not a git repo and no clone URL configured");
    return false;
  }
  if (QStandardPaths::findExecutable(QStringLiteral("git")).isEmpty()) {
    if (p_err) *p_err = QStringLiteral("git not found");
    return false;
  }
  QProcess p;
  p.start(QStringLiteral("git"), cloneArgs(p_repoUrl, p_destDir));
  if (!p.waitForFinished(60000)) {
    if (p_err) *p_err = QStringLiteral("git clone timed out");
    return false;
  }
  if (p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0) {
    if (p_err) *p_err = QString::fromUtf8(p.readAllStandardError());
    return false;
  }
  return true;
}
