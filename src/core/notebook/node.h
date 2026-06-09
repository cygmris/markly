#ifndef NODE_H
#define NODE_H

#include <QDateTime>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVector>

#include "nodevisual.h"
#include <core/global.h>

namespace markly {
class Notebook;

// A node in the notebook tree: a folder (container) or a file (content).
// Concrete class — VNote's Node/VXNode split is merged here since Markly has a
// single bundle/vx format and local backend. Path logic (relative to the notebook
// root, absolute via backend) is folded in.
class Node {
public:
  enum class Type { Folder, File };

  enum { InvalidId = 0 };

  Node(Type p_type, const QString &p_name, Notebook *p_notebook, Node *p_parent);

  ~Node() = default;

  Type getType() const { return m_type; }
  bool isContainer() const { return m_type == Type::Folder; }
  bool hasContent() const { return m_type == Type::File; }

  bool isRoot() const { return m_parent == nullptr; }

  const QString &getName() const { return m_name; }
  void setName(const QString &p_name) { m_name = p_name; }

  ID getId() const { return m_id; }
  void setId(ID p_id) { m_id = p_id; }

  ID getSignature() const { return m_signature; }
  void setSignature(ID p_signature) { m_signature = p_signature; }

  const QDateTime &getCreatedTimeUtc() const { return m_createdTimeUtc; }
  void setCreatedTimeUtc(const QDateTime &p_time) { m_createdTimeUtc = p_time; }

  const QDateTime &getModifiedTimeUtc() const { return m_modifiedTimeUtc; }
  void setModifiedTimeUtc(const QDateTime &p_time) { m_modifiedTimeUtc = p_time; }
  void touchModifiedTime() { m_modifiedTimeUtc = QDateTime::currentDateTimeUtc(); }

  const QStringList &getTags() const { return m_tags; }
  void setTags(const QStringList &p_tags) { m_tags = p_tags; }

  const QString &getAttachmentFolder() const { return m_attachmentFolder; }
  void setAttachmentFolder(const QString &p_folder) { m_attachmentFolder = p_folder; }

  const NodeVisual &getVisual() const { return m_visual; }
  void setVisual(const NodeVisual &p_visual) { m_visual = p_visual; }

  bool isLoaded() const { return m_loaded; }
  void setLoaded(bool p_loaded) { m_loaded = p_loaded; }

  Node *getParent() const { return m_parent; }
  void setParent(Node *p_parent) { m_parent = p_parent; }

  Notebook *getNotebook() const { return m_notebook; }

  const QVector<QSharedPointer<Node>> &getChildren() const { return m_children; }
  int getChildrenCount() const { return m_children.size(); }
  void addChild(const QSharedPointer<Node> &p_node);
  void insertChild(int p_idx, const QSharedPointer<Node> &p_node);
  void removeChild(const QSharedPointer<Node> &p_node);
  void clearChildren() { m_children.clear(); }
  QSharedPointer<Node> findChild(const QString &p_name, bool p_caseSensitive = true) const;
  bool containsChild(const QString &p_name, bool p_caseSensitive = true) const;

  // Path of this node relative to the notebook root ("" for root).
  QString fetchRelativePath() const;

  // Absolute path on disk (via the notebook backend root).
  QString fetchAbsolutePath() const;

  static ID generateSignature();

private:
  Type m_type;
  QString m_name;
  ID m_id = InvalidId;
  ID m_signature = InvalidId;
  QDateTime m_createdTimeUtc;
  QDateTime m_modifiedTimeUtc;
  QStringList m_tags;
  QString m_attachmentFolder;
  NodeVisual m_visual;

  bool m_loaded = false;

  Notebook *m_notebook = nullptr;
  Node *m_parent = nullptr;
  QVector<QSharedPointer<Node>> m_children;
};
} // namespace markly

#endif // NODE_H
