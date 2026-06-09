#include "buffer.h"

#include <utils/fileutils.h>

using namespace markly;

Buffer::Buffer(ID p_id, const QString &p_path, const QString &p_name,
               const QSharedPointer<Node> &p_node, QObject *p_parent)
    : QObject(p_parent), m_id(p_id), m_path(p_path), m_name(p_name), m_node(p_node.toWeakRef()) {}

void Buffer::load() {
  m_content = FileUtils::readTextFile(m_path);
  setDirty(false);
}

void Buffer::save() {
  FileUtils::writeFile(m_path, m_content);
  setDirty(false);
}

void Buffer::setContent(const QString &p_content) {
  if (p_content == m_content) {
    return;
  }
  m_content = p_content;
  emit contentChanged();
  setDirty(true);
}

void Buffer::setDirty(bool p_dirty) {
  if (m_dirty == p_dirty) {
    return;
  }
  m_dirty = p_dirty;
  emit dirtyChanged(m_dirty);
}
