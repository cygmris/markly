#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QByteArray>
#include <QString>

namespace markly {
// Minimal file helpers (foundation subset of VNote's FileUtils).
class FileUtils {
public:
  FileUtils() = delete;

  // Read the whole file as raw bytes. Returns empty on failure.
  static QByteArray readFile(const QString &p_filePath);

  // Read the whole file as UTF-8 text.
  static QString readTextFile(const QString &p_filePath);

  // Write raw bytes to file, creating parent dirs as needed.
  // Throws Exception on failure.
  static void writeFile(const QString &p_filePath, const QByteArray &p_data);

  // Write UTF-8 text to file.
  static void writeFile(const QString &p_filePath, const QString &p_text);

  static bool ensureDirExists(const QString &p_dirPath);

  static bool existsFile(const QString &p_filePath);
};
} // namespace markly

#endif // FILEUTILS_H
