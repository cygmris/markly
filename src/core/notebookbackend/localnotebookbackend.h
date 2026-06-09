#ifndef LOCALNOTEBOOKBACKEND_H
#define LOCALNOTEBOOKBACKEND_H

#include "inotebookbackend.h"

namespace markly {
// Local file-system backend.
class LocalNotebookBackend : public INotebookBackend {
public:
  explicit LocalNotebookBackend(const QString &p_rootPath);

  bool exists(const QString &p_path) const override;
  bool existsFile(const QString &p_path) const override;
  bool existsDir(const QString &p_path) const override;
  bool isEmptyDir(const QString &p_dirPath) const override;

  void makePath(const QString &p_dirPath) override;

  void writeFile(const QString &p_filePath, const QByteArray &p_data) override;
  void writeFile(const QString &p_filePath, const QString &p_text) override;

  QByteArray readFile(const QString &p_filePath) override;
  QString readTextFile(const QString &p_filePath) override;

  void renameFile(const QString &p_filePath, const QString &p_name) override;
  void renameDir(const QString &p_dirPath, const QString &p_name) override;

  void copyFile(const QString &p_filePath, const QString &p_destPath, bool p_move) override;
  void copyDir(const QString &p_dirPath, const QString &p_destPath, bool p_move) override;

  void removeFile(const QString &p_filePath) override;
  void removeDir(const QString &p_dirPath) override;
  bool removeDirIfEmpty(const QString &p_dirPath) override;

  QStringList listDir(const QString &p_dirPath, bool p_files, bool p_dirs) const override;
};
} // namespace markly

#endif // LOCALNOTEBOOKBACKEND_H
