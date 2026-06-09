#include "fileutils.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <core/exception.h>

using namespace markly;

QByteArray FileUtils::readFile(const QString &p_filePath) {
  QFile file(p_filePath);
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "failed to read file" << p_filePath << file.errorString();
    return QByteArray();
  }
  auto data = file.readAll();
  file.close();
  return data;
}

QString FileUtils::readTextFile(const QString &p_filePath) {
  return QString::fromUtf8(readFile(p_filePath));
}

void FileUtils::writeFile(const QString &p_filePath, const QByteArray &p_data) {
  ensureDirExists(QFileInfo(p_filePath).absolutePath());

  QFile file(p_filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    Exception::throwOne(Exception::Type::FailToWriteFile,
                        QStringLiteral("failed to open file to write %1 (%2)")
                            .arg(p_filePath, file.errorString()));
  }
  file.write(p_data);
  file.close();
}

void FileUtils::writeFile(const QString &p_filePath, const QString &p_text) {
  writeFile(p_filePath, p_text.toUtf8());
}

bool FileUtils::ensureDirExists(const QString &p_dirPath) {
  if (p_dirPath.isEmpty()) {
    return false;
  }
  QDir dir(p_dirPath);
  if (dir.exists()) {
    return true;
  }
  return dir.mkpath(p_dirPath);
}

bool FileUtils::existsFile(const QString &p_filePath) {
  QFileInfo fi(p_filePath);
  return fi.exists() && fi.isFile();
}
