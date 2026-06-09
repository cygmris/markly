#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <QString>

namespace markly {
// Minimal path helpers (foundation subset of VNote's PathUtils).
class PathUtils {
public:
  PathUtils() = delete;

  // Clean a path (normalize separators, resolve . and ..).
  static QString cleanPath(const QString &p_path);

  // Join @p_folder and @p_name into one clean path.
  static QString concatenateFilePath(const QString &p_folder, const QString &p_name);

  // Return the parent directory path of @p_path.
  static QString parentDirPath(const QString &p_path);

  // Return the file name (last component) of @p_path.
  static QString fileName(const QString &p_path);
};
} // namespace markly

#endif // PATHUTILS_H
