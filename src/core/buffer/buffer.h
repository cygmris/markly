#ifndef BUFFER_H
#define BUFFER_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QWeakPointer>

#include <core/global.h>

namespace markly {
class Node;

// An open file's in-memory content and edit state. Identified by absolute path.
class Buffer : public QObject {
  Q_OBJECT
public:
  Buffer(ID p_id, const QString &p_path, const QString &p_name,
         const QSharedPointer<Node> &p_node, QObject *p_parent = nullptr);

  ID getId() const { return m_id; }
  const QString &getPath() const { return m_path; }
  const QString &getName() const { return m_name; }
  const QString &getContent() const { return m_content; }
  bool isDirty() const { return m_dirty; }

  // Read the file content from disk.
  void load();

  // Write content to disk and clear the dirty flag. Throws on failure.
  void save();

  // Update content from an editor; marks dirty if changed.
  void setContent(const QString &p_content);

signals:
  void contentChanged();
  void dirtyChanged(bool p_dirty);

private:
  void setDirty(bool p_dirty);

  ID m_id;
  QString m_path;
  QString m_name;
  QString m_content;
  bool m_dirty = false;
  QWeakPointer<Node> m_node;
};
} // namespace markly

#endif // BUFFER_H
