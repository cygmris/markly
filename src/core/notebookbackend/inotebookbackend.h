#ifndef INOTEBOOKBACKEND_H
#define INOTEBOOKBACKEND_H

#include <QByteArray>
#include <QString>
#include <QStringList>

namespace markly {
// Abstract notebook file-system backend. All relative paths resolve against the
// notebook root. Leaner than VNote's INotebookBackend but covers the CRUD needs.
class INotebookBackend {
public:
  explicit INotebookBackend(const QString &p_rootPath) : m_rootPath(p_rootPath) {}
  virtual ~INotebookBackend() = default;

  const QString &getRootPath() const { return m_rootPath; }

  // Resolve a notebook-relative path to an absolute path (absolute input passes through).
  QString getFullPath(const QString &p_path) const;

  // Notebook-relative path of an absolute path under the root.
  QString getRelativePath(const QString &p_path) const;

  virtual bool exists(const QString &p_path) const = 0;
  virtual bool existsFile(const QString &p_path) const = 0;
  virtual bool existsDir(const QString &p_path) const = 0;
  virtual bool isEmptyDir(const QString &p_dirPath) const = 0;

  virtual void makePath(const QString &p_dirPath) = 0;

  virtual void writeFile(const QString &p_filePath, const QByteArray &p_data) = 0;
  virtual void writeFile(const QString &p_filePath, const QString &p_text) = 0;

  virtual QByteArray readFile(const QString &p_filePath) = 0;
  virtual QString readTextFile(const QString &p_filePath) = 0;

  virtual void renameFile(const QString &p_filePath, const QString &p_name) = 0;
  virtual void renameDir(const QString &p_dirPath, const QString &p_name) = 0;

  // @p_filePath may be outside the notebook (for imports). @p_move removes the source.
  virtual void copyFile(const QString &p_filePath, const QString &p_destPath,
                        bool p_move = false) = 0;
  virtual void copyDir(const QString &p_dirPath, const QString &p_destPath,
                       bool p_move = false) = 0;

  virtual void removeFile(const QString &p_filePath) = 0;
  virtual void removeDir(const QString &p_dirPath) = 0;
  virtual bool removeDirIfEmpty(const QString &p_dirPath) = 0;

  virtual QStringList listDir(const QString &p_dirPath, bool p_files, bool p_dirs) const = 0;

protected:
  QString m_rootPath;
};
} // namespace markly

#endif // INOTEBOOKBACKEND_H
