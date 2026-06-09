#ifndef BUFFERMGR_H
#define BUFFERMGR_H

#include <QHash>
#include <QObject>
#include <QSharedPointer>
#include <QVector>

#include <core/global.h>

namespace markly {
class Buffer;
class Node;

// Manages open buffers, de-duplicated by absolute file path.
class BufferMgr : public QObject {
  Q_OBJECT
public:
  explicit BufferMgr(QObject *p_parent = nullptr);

  // Open (or reuse) a buffer for @p_path. Returns null on read failure.
  Buffer *open(const QString &p_path, const QSharedPointer<Node> &p_node = nullptr);

  Buffer *get(ID p_id) const;

  void close(ID p_id);

  QVector<Buffer *> getBuffers() const;

signals:
  void bufferOpened(Buffer *p_buffer);
  void bufferClosed(ID p_id);

private:
  QHash<ID, Buffer *> m_buffers;       // id -> buffer
  QHash<QString, ID> m_pathToId;       // abs path -> id
  ID m_nextId = 1;
};
} // namespace markly

#endif // BUFFERMGR_H
