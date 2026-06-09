#include "buffermgr.h"

#include <QFileInfo>

#include "buffer.h"
#include <core/exception.h>

using namespace markly;

BufferMgr::BufferMgr(QObject *p_parent) : QObject(p_parent) {}

Buffer *BufferMgr::open(const QString &p_path, const QSharedPointer<Node> &p_node) {
  const QString absPath = QFileInfo(p_path).absoluteFilePath();

  // Reuse an existing buffer for this path.
  if (m_pathToId.contains(absPath)) {
    return m_buffers.value(m_pathToId.value(absPath));
  }

  if (!QFileInfo::exists(absPath)) {
    qWarning() << "cannot open non-existent file" << absPath;
    return nullptr;
  }

  const ID id = m_nextId++;
  auto *buffer = new Buffer(id, absPath, QFileInfo(absPath).fileName(), p_node, this);
  try {
    buffer->load();
  } catch (Exception &e) {
    qWarning() << "failed to load buffer" << absPath << e.what();
    delete buffer;
    return nullptr;
  }
  m_buffers.insert(id, buffer);
  m_pathToId.insert(absPath, id);
  emit bufferOpened(buffer);
  return buffer;
}

Buffer *BufferMgr::get(ID p_id) const { return m_buffers.value(p_id); }

void BufferMgr::close(ID p_id) {
  auto *buffer = m_buffers.value(p_id);
  if (!buffer) {
    return;
  }
  m_pathToId.remove(buffer->getPath());
  m_buffers.remove(p_id);
  buffer->deleteLater();
  emit bufferClosed(p_id);
}

QVector<Buffer *> BufferMgr::getBuffers() const {
  QVector<Buffer *> list;
  for (auto *b : m_buffers) {
    list.append(b);
  }
  return list;
}
