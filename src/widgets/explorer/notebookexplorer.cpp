#include "notebookexplorer.h"

#include <QSet>

#include <core/exception.h>
#include <core/marklyapp.h>
#include <core/notebook/node.h>
#include <core/notebook/notebook.h>
#include <core/notebookbackend/inotebookbackend.h>
#include <core/notebookmgr.h>

using namespace markly;

NotebookExplorer::NotebookExplorer(NotebookMgr *p_mgr, QObject *p_parent)
    : QObject(p_parent), m_mgr(p_mgr) {
  if (m_mgr) {
    connect(m_mgr, &NotebookMgr::currentNotebookChanged, this,
            &NotebookExplorer::onCurrentNotebookChanged);
    connect(m_mgr, &NotebookMgr::notebookAdded, this, [this]() { rebuild(); });
    connect(m_mgr, &NotebookMgr::notebookRemoved, this, [this]() { rebuild(); });
  }
  rebuild();
}

void NotebookExplorer::onCurrentNotebookChanged() {
  m_expanded.clear();
  m_selectedId = 0;
  rebuild();
}

QSharedPointer<Notebook> NotebookExplorer::currentNotebook() const {
  return m_mgr ? m_mgr->getCurrentNotebook() : nullptr;
}

bool NotebookExplorer::hasNotebook() const { return !currentNotebook().isNull(); }

QString NotebookExplorer::currentNotebookName() const {
  auto nb = currentNotebook();
  return nb ? nb->getName() : QString();
}

QVariantList NotebookExplorer::notebooks() const {
  QVariantList list;
  if (!m_mgr) {
    return list;
  }
  const auto current = m_mgr->getCurrentNotebook();
  for (const auto &nb : m_mgr->getNotebooks()) {
    QVariantMap m;
    m[QStringLiteral("id")] = static_cast<double>(nb->getId());
    m[QStringLiteral("name")] = nb->getName();
    m[QStringLiteral("current")] = (current && current->getId() == nb->getId());
    list.append(m);
  }
  return list;
}

bool NotebookExplorer::isIgnoredEntry(const QString &p_name) {
  static const QSet<QString> ignored = {
      QStringLiteral("vx_notebook.json"), QStringLiteral("vx.json"),
      QStringLiteral("vx_notebook.db"),   QStringLiteral("vx_images"),
      QStringLiteral("vx_attachments"),   QStringLiteral("vx_recycle_bin")};
  return p_name.startsWith(QLatin1Char('.')) || ignored.contains(p_name);
}

void NotebookExplorer::rebuild() {
  m_visibleNodes.clear();
  m_nodeMap.clear();

  auto nb = currentNotebook();
  if (nb) {
    auto root = nb->getRootNode();
    if (root) {
      nb->loadNodeChildren(root);
      appendChildren(root, 0);
    }
  }
  emit changed();
}

void NotebookExplorer::appendChildren(const QSharedPointer<Node> &p_folder, int p_depth) {
  auto nb = currentNotebook();
  if (!nb) {
    return;
  }
  nb->loadNodeChildren(p_folder);

  for (const auto &child : p_folder->getChildren()) {
    m_nodeMap.insert(child->getId(), child.toWeakRef());

    const bool isFolder = child->isContainer();
    const bool expanded = isFolder && m_expanded.contains(child->getId());

    QVariantMap m;
    m[QStringLiteral("nodeId")] = static_cast<double>(child->getId());
    m[QStringLiteral("parentId")] = static_cast<double>(p_folder->getId());
    m[QStringLiteral("name")] = child->getName();
    m[QStringLiteral("type")] = isFolder ? QStringLiteral("folder") : QStringLiteral("file");
    m[QStringLiteral("depth")] = p_depth;
    m[QStringLiteral("expanded")] = expanded;
    m[QStringLiteral("hasChildren")] = isFolder;
    m[QStringLiteral("selected")] = (child->getId() == m_selectedId);
    m[QStringLiteral("backgroundColor")] = child->getVisual().getBackgroundColor();
    m[QStringLiteral("borderColor")] = child->getVisual().getBorderColor();
    m[QStringLiteral("nameColor")] = child->getVisual().getNameColor();
    m[QStringLiteral("isExternal")] = false;
    m_visibleNodes.append(m);

    if (expanded) {
      appendChildren(child, p_depth + 1);
    }
  }

  appendExternal(p_folder, p_depth);
}

void NotebookExplorer::appendExternal(const QSharedPointer<Node> &p_folder, int p_depth) {
  auto nb = currentNotebook();
  if (!nb) {
    return;
  }
  auto *backend = nb->getBackend();
  const auto folderRel = p_folder->fetchRelativePath();

  QSet<QString> managed;
  for (const auto &child : p_folder->getChildren()) {
    managed.insert(child->getName());
  }

  const auto entries = backend->listDir(folderRel, true, true);
  for (const auto &name : entries) {
    if (isIgnoredEntry(name) || managed.contains(name)) {
      continue;
    }
    const auto rel = folderRel.isEmpty() ? name : (folderRel + QLatin1Char('/') + name);
    const bool isDir = backend->existsDir(rel);

    QVariantMap m;
    m[QStringLiteral("nodeId")] = 0.0;
    m[QStringLiteral("parentId")] = static_cast<double>(p_folder->getId());
    m[QStringLiteral("name")] = name;
    m[QStringLiteral("type")] = isDir ? QStringLiteral("folder") : QStringLiteral("file");
    m[QStringLiteral("depth")] = p_depth;
    m[QStringLiteral("expanded")] = false;
    m[QStringLiteral("hasChildren")] = false;
    m[QStringLiteral("selected")] = false;
    m[QStringLiteral("backgroundColor")] = QString();
    m[QStringLiteral("borderColor")] = QString();
    m[QStringLiteral("nameColor")] = QString();
    m[QStringLiteral("isExternal")] = true;
    m_visibleNodes.append(m);
  }
}

QSharedPointer<Node> NotebookExplorer::resolveNode(ID p_id) const {
  if (p_id == 0) {
    auto nb = currentNotebook();
    return nb ? nb->getRootNode() : nullptr;
  }
  return m_nodeMap.value(p_id).lock();
}

QString NotebookExplorer::validateName(const QSharedPointer<Node> &p_parent,
                                       const QString &p_name) const {
  if (p_name.trimmed().isEmpty()) {
    return tr("名称不能为空");
  }
  if (p_name.contains(QLatin1Char('/')) || p_name.contains(QLatin1Char('\\'))) {
    return tr("名称含非法字符");
  }
  if (p_parent && p_parent->containsChild(p_name)) {
    return tr("已存在同名节点");
  }
  return QString();
}

void NotebookExplorer::toggleExpand(double p_nodeId) {
  const auto id = static_cast<ID>(p_nodeId);
  if (m_expanded.contains(id)) {
    m_expanded.remove(id);
  } else {
    m_expanded.insert(id);
  }
  rebuild();
}

void NotebookExplorer::selectNode(double p_nodeId) {
  const auto id = static_cast<ID>(p_nodeId);
  m_selectedId = id;
  auto node = resolveNode(id);
  if (node && node->hasContent()) {
    emit MarklyApp::getInst().openFileRequested(node->fetchAbsolutePath());
  }
  rebuild();
}

QString NotebookExplorer::newNote(double p_parentId, const QString &p_name) {
  auto nb = currentNotebook();
  auto parent = resolveNode(static_cast<ID>(p_parentId));
  if (!nb || !parent) {
    return tr("无效的父节点");
  }
  const auto err = validateName(parent, p_name);
  if (!err.isEmpty()) {
    return err;
  }
  try {
    nb->newNode(parent, Node::Type::File, p_name);
  } catch (Exception &e) {
    return QString::fromUtf8(e.what());
  }
  m_expanded.insert(parent->getId());
  rebuild();
  return QString();
}

QString NotebookExplorer::newFolder(double p_parentId, const QString &p_name) {
  auto nb = currentNotebook();
  auto parent = resolveNode(static_cast<ID>(p_parentId));
  if (!nb || !parent) {
    return tr("无效的父节点");
  }
  const auto err = validateName(parent, p_name);
  if (!err.isEmpty()) {
    return err;
  }
  try {
    nb->newNode(parent, Node::Type::Folder, p_name);
  } catch (Exception &e) {
    return QString::fromUtf8(e.what());
  }
  m_expanded.insert(parent->getId());
  rebuild();
  return QString();
}

QString NotebookExplorer::renameNode(double p_nodeId, const QString &p_name) {
  auto nb = currentNotebook();
  auto node = resolveNode(static_cast<ID>(p_nodeId));
  if (!nb || !node || node->isRoot()) {
    return tr("无效的节点");
  }
  if (p_name.trimmed().isEmpty()) {
    return tr("名称不能为空");
  }
  if (p_name.contains(QLatin1Char('/')) || p_name.contains(QLatin1Char('\\'))) {
    return tr("名称含非法字符");
  }
  // Duplicate among siblings (excluding self).
  if (node->getName() != p_name && node->getParent() &&
      node->getParent()->containsChild(p_name)) {
    return tr("已存在同名节点");
  }
  try {
    nb->renameNode(node, p_name);
  } catch (Exception &e) {
    return QString::fromUtf8(e.what());
  }
  rebuild();
  return QString();
}

void NotebookExplorer::removeToRecycle(double p_nodeId) {
  auto nb = currentNotebook();
  auto node = resolveNode(static_cast<ID>(p_nodeId));
  if (nb && node && !node->isRoot()) {
    try {
      nb->moveNodeToRecycleBin(node);
    } catch (Exception &e) {
      qWarning() << "removeToRecycle failed" << e.what();
    }
    rebuild();
  }
}

void NotebookExplorer::removePermanent(double p_nodeId) {
  auto nb = currentNotebook();
  auto node = resolveNode(static_cast<ID>(p_nodeId));
  if (nb && node && !node->isRoot()) {
    try {
      nb->removeNode(node);
    } catch (Exception &e) {
      qWarning() << "removePermanent failed" << e.what();
    }
    rebuild();
  }
}

void NotebookExplorer::setNodeColor(double p_nodeId, const QString &p_bg, const QString &p_border,
                                    const QString &p_name) {
  auto nb = currentNotebook();
  auto node = resolveNode(static_cast<ID>(p_nodeId));
  if (nb && node) {
    nb->updateNodeVisual(node, NodeVisual(p_bg, p_border, p_name));
    rebuild();
  }
}

void NotebookExplorer::clearNodeColor(double p_nodeId) {
  setNodeColor(p_nodeId, QString(), QString(), QString());
}

void NotebookExplorer::importExternal(double p_parentId, const QString &p_name) {
  auto nb = currentNotebook();
  auto parent = resolveNode(static_cast<ID>(p_parentId));
  if (!nb || !parent) {
    return;
  }
  const auto rel = parent->fetchRelativePath().isEmpty()
                       ? p_name
                       : (parent->fetchRelativePath() + QLatin1Char('/') + p_name);
  const auto type =
      nb->getBackend()->existsDir(rel) ? Node::Type::Folder : Node::Type::File;
  try {
    nb->importNode(parent, type, p_name);
  } catch (Exception &e) {
    qWarning() << "importExternal failed" << e.what();
  }
  rebuild();
}

void NotebookExplorer::switchNotebook(double p_notebookId) {
  if (m_mgr) {
    m_mgr->setCurrentNotebook(static_cast<ID>(p_notebookId));
  }
}

bool NotebookExplorer::newNotebookAt(const QString &p_rootDir, const QString &p_name,
                                     const QString &p_description) {
  if (!m_mgr || p_rootDir.isEmpty() || p_name.trimmed().isEmpty()) {
    return false;
  }
  try {
    return !m_mgr->newNotebook(p_rootDir, p_name, p_description).isNull();
  } catch (Exception &e) {
    qWarning() << "newNotebook failed" << e.what();
    return false;
  }
}

bool NotebookExplorer::openNotebookAt(const QString &p_rootDir) {
  if (!m_mgr || p_rootDir.isEmpty()) {
    return false;
  }
  return !m_mgr->openNotebook(p_rootDir).isNull();
}
