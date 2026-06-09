#ifndef NOTEBOOKMGR_H
#define NOTEBOOKMGR_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QVector>

#include "global.h"
#include "noncopyable.h"

namespace markly {
class Notebook;

// Registry of open notebooks + the current notebook. Assembles BundleNotebook
// (local backend + vx config + sqlite index). Persists the open list + current to
// the session config, restoring on startup.
class NotebookMgr : public QObject, private Noncopyable {
  Q_OBJECT
public:
  explicit NotebookMgr(QObject *p_parent = nullptr);

  // Restore notebooks recorded in the session config.
  void loadNotebooks();

  // Create a brand-new notebook at @p_rootDir.
  QSharedPointer<Notebook> newNotebook(const QString &p_rootDir, const QString &p_name,
                                       const QString &p_description);

  // Open an existing notebook directory (must contain vx_notebook.json).
  QSharedPointer<Notebook> openNotebook(const QString &p_rootDir);

  void closeNotebook(ID p_id);

  const QVector<QSharedPointer<Notebook>> &getNotebooks() const { return m_notebooks; }

  QSharedPointer<Notebook> getNotebook(ID p_id) const;

  QSharedPointer<Notebook> getCurrentNotebook() const;
  void setCurrentNotebook(ID p_id);

signals:
  void notebookAdded(Notebook *p_notebook);
  void notebookRemoved(ID p_id);
  void currentNotebookChanged(Notebook *p_notebook);

private:
  void persist();

  QVector<QSharedPointer<Notebook>> m_notebooks;
  ID m_currentId = 0;
  ID m_nextId = 1;
};
} // namespace markly

#endif // NOTEBOOKMGR_H
