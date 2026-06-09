#ifndef MARKLY_TASKMGR_H
#define MARKLY_TASKMGR_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

namespace markly {
// An external command task: command line with ${var} placeholders, optional args, and
// whether to run via a shell (sh -c).
struct Task {
  QString m_name;
  QString m_command;
  QStringList m_args;
  bool m_shell = true;
};

// Context for expanding task variables (the active note + notebook).
struct TaskContext {
  QString m_bufferPath;
  QString m_bufferDir;
  QString m_bufferName;
  QString m_notebookFolder;
};

// Manages external command tasks stored as JSON under <AppConfigLocation>/tasks/.
// Runs them via QProcess with variable expansion. markly_core for unit testing.
class TaskMgr : public QObject {
  Q_OBJECT
public:
  explicit TaskMgr(QObject *p_parent = nullptr);

  void load();
  QList<Task> tasks() const;
  Task get(const QString &p_name) const { return m_tasks.value(p_name); }

  // Expand ${bufferPath}/${bufferName}/${bufferBaseName}/${bufferDir}/${notebookFolder}/
  // ${cwd}. Unknown ${...} kept; missing value -> empty.
  static QString expand(const QString &p_text, const TaskContext &p_ctx);

  // Run the named task synchronously (timeout p_timeoutMs); returns stdout+stderr,
  // writes the process exit code to p_outExit (-1 on failure to start/timeout).
  QString run(const QString &p_name, const TaskContext &p_ctx, int &p_outExit,
              int p_timeoutMs = 10000) const;

signals:
  void changed();

private:
  void seedDefaults();
  QString dir() const;

  QHash<QString, Task> m_tasks;
};
} // namespace markly

#endif // MARKLY_TASKMGR_H
