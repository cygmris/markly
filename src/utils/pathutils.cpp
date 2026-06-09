#include "pathutils.h"

#include <QDir>
#include <QFileInfo>

using namespace markly;

QString PathUtils::cleanPath(const QString &p_path) { return QDir::cleanPath(p_path); }

QString PathUtils::concatenateFilePath(const QString &p_folder, const QString &p_name) {
  if (p_name.isEmpty()) {
    return cleanPath(p_folder);
  }
  if (p_folder.isEmpty()) {
    return cleanPath(p_name);
  }
  return cleanPath(p_folder + QLatin1Char('/') + p_name);
}

QString PathUtils::parentDirPath(const QString &p_path) {
  QFileInfo fi(p_path);
  return cleanPath(fi.absolutePath());
}

QString PathUtils::fileName(const QString &p_path) {
  return QFileInfo(p_path).fileName();
}
