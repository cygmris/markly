#ifndef MARKLY_TASKBRIDGE_H
#define MARKLY_TASKBRIDGE_H

#include <QObject>
#include <QString>
#include <QVariantList>

namespace markly {
class TaskMgr;
class NotebookMgr;

// Bridge for external command tasks (context property "Tasks"). markly_core.
class TaskBridge : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList list READ list NOTIFY changed)
public:
  explicit TaskBridge(TaskMgr *p_taskMgr, NotebookMgr *p_notebookMgr,
                      QObject *p_parent = nullptr);

  QVariantList list() const; // [{name}]

  // Run a task with the active note's dir/name as context; returns "[退出码 N]\n" + output.
  Q_INVOKABLE QString run(const QString &p_name, const QString &p_bufferDir,
                          const QString &p_bufferName);

signals:
  void changed();

private:
  TaskMgr *m_taskMgr = nullptr;
  NotebookMgr *m_notebookMgr = nullptr;
};
} // namespace markly

#endif // MARKLY_TASKBRIDGE_H
