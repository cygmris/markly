#include "tagbridge.h"

#include <QHash>
#include <QVariantMap>

#include <core/marklyapp.h>
#include <core/notebook/node.h>
#include <core/notebook/notebook.h>
#include <core/notebook/notebookdatabaseaccess.h>
#include <core/notebook/notebooktagmgr.h>
#include <core/notebookmgr.h>

using namespace markly;

TagBridge::TagBridge(NotebookMgr *p_mgr, QObject *p_parent)
    : QObject(p_parent), m_mgr(p_mgr) {
  if (m_mgr) {
    connect(m_mgr, &NotebookMgr::currentNotebookChanged, this, [this]() { refresh(); });
  }
  connect(&MarklyApp::getInst(), &MarklyApp::tagsChanged, this, [this]() { refresh(); });
  refresh();
}

void TagBridge::refresh() {
  m_tags.clear();
  auto nb = m_mgr ? m_mgr->getCurrentNotebook() : nullptr;
  if (nb && nb->getTagMgr() && nb->getDatabase()) {
    const auto names = nb->getTagMgr()->getTags();
    for (const auto &name : names) {
      QVariantMap m;
      m[QStringLiteral("name")] = name;
      m[QStringLiteral("count")] = nb->getDatabase()->queryNodesByTag(name).size();
      m_tags.append(m);
    }
  }
  // Keep the selection only if the tag still exists.
  bool stillThere = false;
  for (const auto &t : m_tags) {
    if (t.toMap().value(QStringLiteral("name")).toString() == m_selectedTag) {
      stillThere = true;
      break;
    }
  }
  if (!stillThere) {
    m_selectedTag.clear();
  }
  rebuildTaggedNodes();
  emit changed();
}

void TagBridge::selectTag(const QString &p_name) {
  m_selectedTag = p_name;
  rebuildTaggedNodes();
  emit changed();
}

void TagBridge::rebuildTaggedNodes() {
  m_taggedNodes.clear();
  auto nb = m_mgr ? m_mgr->getCurrentNotebook() : nullptr;
  if (!nb || !nb->getDatabase() || m_selectedTag.isEmpty()) {
    return;
  }
  const auto ids = nb->getDatabase()->queryNodesByTag(m_selectedTag);
  // Resolve node ids to name/path via the markdown-node map (same as Searcher).
  QHash<ID, QSharedPointer<Node>> byId;
  for (const auto &n : nb->collectMarkdownNodes()) {
    byId.insert(n->getId(), n);
  }
  for (const auto id : ids) {
    auto n = byId.value(id);
    if (!n) {
      continue;
    }
    QVariantMap m;
    m[QStringLiteral("name")] = n->getName();
    m[QStringLiteral("path")] = n->fetchAbsolutePath();
    m_taggedNodes.append(m);
  }
}

void TagBridge::openTaggedNode(int p_index) {
  if (p_index < 0 || p_index >= m_taggedNodes.size()) {
    return;
  }
  const auto path = m_taggedNodes.at(p_index).toMap().value(QStringLiteral("path")).toString();
  emit MarklyApp::getInst().openFileRequested(path);
}
