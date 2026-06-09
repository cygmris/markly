#include "localnotebookbackend.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <core/exception.h>
#include <utils/fileutils.h>
#include <utils/pathutils.h>

using namespace markly;

// ---- INotebookBackend non-virtual helpers ----
QString INotebookBackend::getFullPath(const QString &p_path) const {
  if (QFileInfo(p_path).isAbsolute()) {
    return p_path;
  }
  return PathUtils::concatenateFilePath(m_rootPath, p_path);
}

QString INotebookBackend::getRelativePath(const QString &p_path) const {
  return QDir(m_rootPath).relativeFilePath(p_path);
}

// ---- LocalNotebookBackend ----
LocalNotebookBackend::LocalNotebookBackend(const QString &p_rootPath)
    : INotebookBackend(PathUtils::cleanPath(p_rootPath)) {}

bool LocalNotebookBackend::exists(const QString &p_path) const {
  return QFileInfo::exists(getFullPath(p_path));
}

bool LocalNotebookBackend::existsFile(const QString &p_path) const {
  QFileInfo fi(getFullPath(p_path));
  return fi.exists() && fi.isFile();
}

bool LocalNotebookBackend::existsDir(const QString &p_path) const {
  QFileInfo fi(getFullPath(p_path));
  return fi.exists() && fi.isDir();
}

bool LocalNotebookBackend::isEmptyDir(const QString &p_dirPath) const {
  QDir dir(getFullPath(p_dirPath));
  return dir.exists() && dir.isEmpty(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
}

void LocalNotebookBackend::makePath(const QString &p_dirPath) {
  const auto full = getFullPath(p_dirPath);
  if (!QDir().mkpath(full)) {
    Exception::throwOne(Exception::Type::FailToCreateDir,
                        QStringLiteral("failed to make path %1").arg(full));
  }
}

void LocalNotebookBackend::writeFile(const QString &p_filePath, const QByteArray &p_data) {
  FileUtils::writeFile(getFullPath(p_filePath), p_data);
}

void LocalNotebookBackend::writeFile(const QString &p_filePath, const QString &p_text) {
  FileUtils::writeFile(getFullPath(p_filePath), p_text);
}

QByteArray LocalNotebookBackend::readFile(const QString &p_filePath) {
  return FileUtils::readFile(getFullPath(p_filePath));
}

QString LocalNotebookBackend::readTextFile(const QString &p_filePath) {
  return FileUtils::readTextFile(getFullPath(p_filePath));
}

void LocalNotebookBackend::renameFile(const QString &p_filePath, const QString &p_name) {
  const auto src = getFullPath(p_filePath);
  const auto dest = PathUtils::concatenateFilePath(QFileInfo(src).absolutePath(), p_name);
  if (!QFile::rename(src, dest)) {
    Exception::throwOne(Exception::Type::FailToRenameFile,
                        QStringLiteral("failed to rename %1 to %2").arg(src, dest));
  }
}

void LocalNotebookBackend::renameDir(const QString &p_dirPath, const QString &p_name) {
  renameFile(p_dirPath, p_name);
}

void LocalNotebookBackend::copyFile(const QString &p_filePath, const QString &p_destPath,
                                    bool p_move) {
  const auto src = getFullPath(p_filePath);
  const auto dest = getFullPath(p_destPath);
  if (src == dest) {
    return;
  }
  QDir().mkpath(QFileInfo(dest).absolutePath());
  if (QFile::exists(dest)) {
    QFile::remove(dest);
  }
  if (!QFile::copy(src, dest)) {
    Exception::throwOne(Exception::Type::FailToCopyFile,
                        QStringLiteral("failed to copy %1 to %2").arg(src, dest));
  }
  if (p_move) {
    QFile::remove(src);
  }
}

void LocalNotebookBackend::copyDir(const QString &p_dirPath, const QString &p_destPath,
                                   bool p_move) {
  const auto src = getFullPath(p_dirPath);
  const auto dest = getFullPath(p_destPath);
  if (src == dest) {
    return;
  }
  QDir().mkpath(dest);
  QDir srcDir(src);
  const auto entries =
      srcDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
  for (const auto &entry : entries) {
    const auto childDest = PathUtils::concatenateFilePath(dest, entry.fileName());
    if (entry.isDir()) {
      copyDir(entry.absoluteFilePath(), childDest, false);
    } else {
      QDir().mkpath(QFileInfo(childDest).absolutePath());
      if (QFile::exists(childDest)) {
        QFile::remove(childDest);
      }
      if (!QFile::copy(entry.absoluteFilePath(), childDest)) {
        Exception::throwOne(
            Exception::Type::FailToCopyFile,
            QStringLiteral("failed to copy %1").arg(entry.absoluteFilePath()));
      }
    }
  }
  if (p_move) {
    QDir(src).removeRecursively();
  }
}

void LocalNotebookBackend::removeFile(const QString &p_filePath) {
  const auto full = getFullPath(p_filePath);
  if (QFile::exists(full) && !QFile::remove(full)) {
    Exception::throwOne(Exception::Type::FailToRemoveFile,
                        QStringLiteral("failed to remove %1").arg(full));
  }
}

void LocalNotebookBackend::removeDir(const QString &p_dirPath) {
  const auto full = getFullPath(p_dirPath);
  QDir dir(full);
  if (dir.exists() && !dir.removeRecursively()) {
    Exception::throwOne(Exception::Type::FailToRemoveDir,
                        QStringLiteral("failed to remove dir %1").arg(full));
  }
}

bool LocalNotebookBackend::removeDirIfEmpty(const QString &p_dirPath) {
  if (!isEmptyDir(p_dirPath)) {
    return false;
  }
  return QDir().rmdir(getFullPath(p_dirPath));
}

QStringList LocalNotebookBackend::listDir(const QString &p_dirPath, bool p_files,
                                          bool p_dirs) const {
  QDir::Filters filters = QDir::NoDotAndDotDot;
  if (p_files) {
    filters |= QDir::Files;
  }
  if (p_dirs) {
    filters |= QDir::Dirs;
  }
  return QDir(getFullPath(p_dirPath)).entryList(filters, QDir::Name);
}
