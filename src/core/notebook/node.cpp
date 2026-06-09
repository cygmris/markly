#include "node.h"

#include <QRandomGenerator>

#include "notebook.h"
#include <utils/pathutils.h>

using namespace markly;

Node::Node(Type p_type, const QString &p_name, Notebook *p_notebook, Node *p_parent)
    : m_type(p_type), m_name(p_name), m_notebook(p_notebook), m_parent(p_parent) {}

void Node::addChild(const QSharedPointer<Node> &p_node) {
  if (!p_node) {
    return;
  }
  p_node->setParent(this);
  m_children.append(p_node);
}

void Node::insertChild(int p_idx, const QSharedPointer<Node> &p_node) {
  if (!p_node) {
    return;
  }
  p_node->setParent(this);
  m_children.insert(qBound(0, p_idx, m_children.size()), p_node);
}

void Node::removeChild(const QSharedPointer<Node> &p_node) {
  m_children.removeOne(p_node);
}

QSharedPointer<Node> Node::findChild(const QString &p_name, bool p_caseSensitive) const {
  const auto cs = p_caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
  for (const auto &child : m_children) {
    if (child->getName().compare(p_name, cs) == 0) {
      return child;
    }
  }
  return nullptr;
}

bool Node::containsChild(const QString &p_name, bool p_caseSensitive) const {
  return !findChild(p_name, p_caseSensitive).isNull();
}

QString Node::fetchRelativePath() const {
  if (isRoot()) {
    return QString();
  }
  QStringList parts;
  const Node *cur = this;
  while (cur && !cur->isRoot()) {
    parts.prepend(cur->getName());
    cur = cur->getParent();
  }
  return parts.join(QLatin1Char('/'));
}

QString Node::fetchAbsolutePath() const {
  const auto rel = fetchRelativePath();
  const auto root = m_notebook ? m_notebook->getRootPath() : QString();
  return rel.isEmpty() ? root : PathUtils::concatenateFilePath(root, rel);
}

ID Node::generateSignature() {
  ID sig = 0;
  while (sig == 0) {
    sig = static_cast<ID>(QRandomGenerator::global()->generate64());
  }
  return sig;
}
