#include "notebook.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

#include "../notebookbackend/inotebookbackend.h"
#include "notebookdatabaseaccess.h"
#include <core/exception.h>
#include <utils/pathutils.h>

using namespace markly;
using namespace markly::vx_node_config;

const QString Notebook::c_notebookConfigName = QStringLiteral("vx_notebook.json");
const QString Notebook::c_nodeConfigName = QStringLiteral("vx.json");
const QString Notebook::c_recycleBinFolder = QStringLiteral("vx_recycle_bin");
const QString Notebook::c_databaseName = QStringLiteral("vx_notebook.db");

namespace {
QString joinRel(const QString &p_base, const QString &p_name) {
  return p_base.isEmpty() ? p_name : (p_base + QLatin1Char('/') + p_name);
}
} // namespace

Notebook::Notebook(ID p_id, const QString &p_rootPath, INotebookBackend *p_backend)
    : m_id(p_id), m_rootPath(PathUtils::cleanPath(p_rootPath)), m_backend(p_backend) {}

Notebook::~Notebook() {
  if (m_db) {
    m_db->close();
  }
}

static QString connectionNameFor(ID p_id) {
  return QStringLiteral("markly_nb_%1").arg(p_id);
}

void Notebook::open() {
  // Notebook config.
  const auto bytes = m_backend->readFile(c_notebookConfigName);
  m_config.fromJson(QJsonDocument::fromJson(bytes).object());

  // Database index.
  m_db.reset(new NotebookDatabaseAccess(m_backend->getFullPath(c_databaseName),
                                        connectionNameFor(m_id)));
  if (!m_db->open()) {
    Exception::throwOne(Exception::Type::FailToReadFile,
                        QStringLiteral("failed to open notebook database"));
  }
  m_db->createTables();

  m_tagMgr.reset(new NotebookTagMgr());
  m_tagMgr->load(m_db.data(), m_config.m_tagGraph);
  m_vc.reset(new DummyVersionController());

  // Root node (read its own vx.json for top-level id/signature/times).
  auto rootCfg = NodeConfig();
  if (m_backend->existsFile(c_nodeConfigName)) {
    rootCfg.fromJson(QJsonDocument::fromJson(m_backend->readFile(c_nodeConfigName)).object());
  }
  m_root = QSharedPointer<Node>::create(Node::Type::Folder, m_config.m_name, this, nullptr);
  m_root->setId(rootCfg.m_id == 0 ? 1 : rootCfg.m_id);
  m_root->setSignature(rootCfg.m_signature);
  m_root->setCreatedTimeUtc(rootCfg.m_createdTimeUtc);
  m_root->setModifiedTimeUtc(rootCfg.m_modifiedTimeUtc);

  // Build the index from disk if absent.
  if (!m_db->nodeExists(m_root->getId())) {
    rebuildDatabase();
    // Tags were just written by the rebuild; reload them into the tag manager.
    m_tagMgr->load(m_db.data(), m_config.m_tagGraph);
  }
}

void Notebook::createOnDisk(const QString &p_name, const QString &p_description) {
  m_config = NotebookConfig();
  m_config.m_name = p_name;
  m_config.m_description = p_description;
  m_config.m_createdTimeUtc = QDateTime::currentDateTimeUtc();

  m_backend->makePath(QString());
  m_backend->writeFile(c_notebookConfigName,
                       QJsonDocument(m_config.toJson()).toJson());

  m_db.reset(new NotebookDatabaseAccess(m_backend->getFullPath(c_databaseName),
                                        connectionNameFor(m_id)));
  if (!m_db->open()) {
    Exception::throwOne(Exception::Type::FailToWriteFile,
                        QStringLiteral("failed to create notebook database"));
  }
  m_db->createTables();

  m_tagMgr.reset(new NotebookTagMgr());
  m_tagMgr->load(m_db.data(), QString());
  m_vc.reset(new DummyVersionController());

  const auto now = QDateTime::currentDateTimeUtc();
  const auto sig = Node::generateSignature();
  const ID rootId = m_db->insertNode(p_name, sig, 0);

  m_root = QSharedPointer<Node>::create(Node::Type::Folder, p_name, this, nullptr);
  m_root->setId(rootId == 0 ? 1 : rootId);
  m_root->setSignature(sig);
  m_root->setCreatedTimeUtc(now);
  m_root->setModifiedTimeUtc(now);
  m_root->setLoaded(true);

  writeNodeConfig(m_root.data());
}

QString Notebook::nodeConfigRelPath(const Node *p_folder) const {
  const auto rel = p_folder->fetchRelativePath();
  return joinRel(rel, c_nodeConfigName);
}

NodeConfig Notebook::readNodeConfig(const Node *p_folder) const {
  NodeConfig cfg;
  const auto path = nodeConfigRelPath(p_folder);
  if (m_backend->existsFile(path)) {
    cfg.fromJson(QJsonDocument::fromJson(m_backend->readFile(path)).object());
  }
  return cfg;
}

void Notebook::writeNodeConfig(Node *p_folder) {
  NodeConfig cfg;
  cfg.m_version = 1;
  cfg.m_id = p_folder->getId();
  cfg.m_signature = p_folder->getSignature();
  cfg.m_createdTimeUtc = p_folder->getCreatedTimeUtc();
  cfg.m_modifiedTimeUtc = p_folder->getModifiedTimeUtc();
  cfg.m_backgroundColor = p_folder->getVisual().getBackgroundColor();
  cfg.m_borderColor = p_folder->getVisual().getBorderColor();
  cfg.m_nameColor = p_folder->getVisual().getNameColor();

  for (const auto &child : p_folder->getChildren()) {
    if (child->hasContent()) {
      NodeFileConfig f;
      f.m_name = child->getName();
      f.m_id = child->getId();
      f.m_signature = child->getSignature();
      f.m_createdTimeUtc = child->getCreatedTimeUtc();
      f.m_modifiedTimeUtc = child->getModifiedTimeUtc();
      f.m_attachmentFolder = child->getAttachmentFolder();
      f.m_tags = child->getTags();
      f.m_backgroundColor = child->getVisual().getBackgroundColor();
      f.m_borderColor = child->getVisual().getBorderColor();
      f.m_nameColor = child->getVisual().getNameColor();
      cfg.m_files.append(f);
    } else {
      NodeFolderConfig f;
      f.m_name = child->getName();
      f.m_backgroundColor = child->getVisual().getBackgroundColor();
      f.m_borderColor = child->getVisual().getBorderColor();
      f.m_nameColor = child->getVisual().getNameColor();
      cfg.m_folders.append(f);
    }
  }

  m_backend->writeFile(nodeConfigRelPath(p_folder), QJsonDocument(cfg.toJson()).toJson());
}

void Notebook::ensureLoaded(const QSharedPointer<Node> &p_node) {
  if (p_node->isContainer() && !p_node->isLoaded()) {
    loadNodeChildren(p_node);
  }
}

void Notebook::collectMarkdownNodes(const QSharedPointer<Node> &p_node,
                                    QVector<QSharedPointer<Node>> &p_out) {
  if (p_node->isContainer()) {
    ensureLoaded(p_node);
    for (const auto &child : p_node->getChildren()) {
      collectMarkdownNodes(child, p_out);
    }
  } else if (p_node->getName().endsWith(QStringLiteral(".md"), Qt::CaseInsensitive)) {
    p_out.append(p_node);
  }
}

QVector<QSharedPointer<Node>> Notebook::collectMarkdownNodes() {
  QVector<QSharedPointer<Node>> out;
  if (m_root) {
    collectMarkdownNodes(m_root, out);
  }
  return out;
}

void Notebook::loadNodeChildren(const QSharedPointer<Node> &p_node) {
  if (!p_node->isContainer() || p_node->isLoaded()) {
    return;
  }
  const auto cfg = readNodeConfig(p_node.data());
  // Folder's own info from its vx.json top level.
  if (cfg.m_id != 0) {
    p_node->setId(cfg.m_id);
  }
  if (cfg.m_signature != 0) {
    p_node->setSignature(cfg.m_signature);
  }
  if (cfg.m_createdTimeUtc.isValid()) {
    p_node->setCreatedTimeUtc(cfg.m_createdTimeUtc);
  }
  if (cfg.m_modifiedTimeUtc.isValid()) {
    p_node->setModifiedTimeUtc(cfg.m_modifiedTimeUtc);
  }
  p_node->setVisual(NodeVisual(cfg.m_backgroundColor, cfg.m_borderColor, cfg.m_nameColor));

  p_node->clearChildren();
  for (const auto &f : cfg.m_files) {
    auto file = QSharedPointer<Node>::create(Node::Type::File, f.m_name, this, p_node.data());
    file->setId(f.m_id);
    file->setSignature(f.m_signature);
    file->setCreatedTimeUtc(f.m_createdTimeUtc);
    file->setModifiedTimeUtc(f.m_modifiedTimeUtc);
    file->setAttachmentFolder(f.m_attachmentFolder);
    file->setTags(f.m_tags);
    file->setVisual(NodeVisual(f.m_backgroundColor, f.m_borderColor, f.m_nameColor));
    file->setLoaded(true);
    p_node->addChild(file);
  }
  for (const auto &f : cfg.m_folders) {
    auto folder = QSharedPointer<Node>::create(Node::Type::Folder, f.m_name, this, p_node.data());
    folder->setVisual(NodeVisual(f.m_backgroundColor, f.m_borderColor, f.m_nameColor));
    folder->setLoaded(false); // own vx.json read on expand
    p_node->addChild(folder);
  }
  p_node->setLoaded(true);
}

QSharedPointer<Node> Notebook::newNode(const QSharedPointer<Node> &p_parent, Node::Type p_type,
                                       const QString &p_name) {
  ensureLoaded(p_parent);

  const auto now = QDateTime::currentDateTimeUtc();
  const auto sig = Node::generateSignature();
  const ID id = m_db->insertNode(p_name, sig, p_parent->getId());

  auto child = QSharedPointer<Node>::create(p_type, p_name, this, p_parent.data());
  child->setId(id);
  child->setSignature(sig);
  child->setCreatedTimeUtc(now);
  child->setModifiedTimeUtc(now);
  child->setLoaded(true);

  const auto childRel = joinRel(p_parent->fetchRelativePath(), p_name);
  if (p_type == Node::Type::File) {
    m_backend->writeFile(childRel, QString());
  } else {
    m_backend->makePath(childRel);
    writeNodeConfig(child.data()); // child folder's own (empty) vx.json
  }

  p_parent->addChild(child);
  writeNodeConfig(p_parent.data());
  return child;
}

QSharedPointer<Node> Notebook::importNode(const QSharedPointer<Node> &p_parent, Node::Type p_type,
                                          const QString &p_name) {
  ensureLoaded(p_parent);

  const auto childRel = joinRel(p_parent->fetchRelativePath(), p_name);
  if (!m_backend->exists(childRel)) {
    return nullptr;
  }

  const auto now = QDateTime::currentDateTimeUtc();
  const auto sig = Node::generateSignature();
  const ID id = m_db->insertNode(p_name, sig, p_parent->getId());

  auto child = QSharedPointer<Node>::create(p_type, p_name, this, p_parent.data());
  child->setId(id);
  child->setSignature(sig);
  child->setCreatedTimeUtc(now);
  child->setModifiedTimeUtc(now);
  child->setLoaded(p_type == Node::Type::File);

  // A folder being imported needs its own vx.json (its disk contents stay external
  // until individually imported).
  if (p_type == Node::Type::Folder &&
      !m_backend->existsFile(joinRel(childRel, c_nodeConfigName))) {
    writeNodeConfig(child.data());
  }

  p_parent->addChild(child);
  writeNodeConfig(p_parent.data());
  return child;
}

void Notebook::renameNode(const QSharedPointer<Node> &p_node, const QString &p_newName) {
  if (p_node->isRoot()) {
    return;
  }
  const auto rel = p_node->fetchRelativePath();
  if (p_node->isContainer()) {
    m_backend->renameDir(rel, p_newName);
  } else {
    m_backend->renameFile(rel, p_newName);
  }
  p_node->setName(p_newName);
  p_node->touchModifiedTime();

  NotebookDatabaseAccess::NodeRow row;
  row.m_id = p_node->getId();
  row.m_name = p_newName;
  row.m_signature = p_node->getSignature();
  row.m_parentId = p_node->getParent() ? p_node->getParent()->getId() : 0;
  m_db->updateNode(row);
  // Name/path changed -> drop the stale FTS row; it is re-indexed on next search.
  m_db->ftsRemove(p_node->getId());

  if (p_node->getParent()) {
    writeNodeConfig(p_node->getParent());
  }
}

void Notebook::removeNode(const QSharedPointer<Node> &p_node) {
  if (p_node->isRoot()) {
    return;
  }
  const auto rel = p_node->fetchRelativePath();
  if (p_node->isContainer()) {
    m_backend->removeDir(rel);
  } else {
    m_backend->removeFile(rel);
  }
  m_db->removeNode(p_node->getId()); // CASCADE removes descendants + tag_node
  m_db->ftsRemove(p_node->getId());

  auto *parent = p_node->getParent();
  if (parent) {
    parent->removeChild(p_node);
    writeNodeConfig(parent);
  }
}

void Notebook::moveNodeToRecycleBin(const QSharedPointer<Node> &p_node) {
  if (p_node->isRoot()) {
    return;
  }
  const auto rel = p_node->fetchRelativePath();
  const auto dayFolder = joinRel(c_recycleBinFolder,
                                 QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd")));
  m_backend->makePath(dayFolder);
  const auto dest = joinRel(dayFolder, p_node->getName());
  if (p_node->isContainer()) {
    m_backend->copyDir(rel, dest, true);
  } else {
    m_backend->copyFile(rel, dest, true);
  }
  m_db->removeNode(p_node->getId());
  m_db->ftsRemove(p_node->getId());

  auto *parent = p_node->getParent();
  if (parent) {
    parent->removeChild(p_node);
    writeNodeConfig(parent);
  }
}

void Notebook::updateNodeVisual(const QSharedPointer<Node> &p_node, const NodeVisual &p_visual) {
  p_node->setVisual(p_visual);
  p_node->touchModifiedTime();
  if (p_node->isContainer() && !p_node->isRoot()) {
    // Folder: write its own vx.json top-level (and the parent's folders[] entry).
    ensureLoaded(p_node);
    writeNodeConfig(p_node.data());
  }
  if (p_node->getParent()) {
    writeNodeConfig(p_node->getParent());
  } else {
    writeNodeConfig(p_node.data()); // root
  }
}

void Notebook::updateNodeTags(const QSharedPointer<Node> &p_node, const QStringList &p_tags) {
  p_node->setTags(p_tags);
  p_node->touchModifiedTime();

  m_db->removeNodeTags(p_node->getId());
  for (const auto &tag : p_tags) {
    m_tagMgr->addTag(m_db.data(), tag, QString());
    m_db->addNodeTag(p_node->getId(), tag);
  }
  // Keep the notebook tag_graph in sync.
  m_config.m_tagGraph = m_tagMgr->toTagGraph();
  saveConfig();

  if (p_node->getParent()) {
    writeNodeConfig(p_node->getParent());
  }
}

void Notebook::saveConfig() {
  m_backend->writeFile(c_notebookConfigName, QJsonDocument(m_config.toJson()).toJson());
}

void Notebook::rebuildDatabase() {
  rebuildDatabaseRecur(QString(), m_root->getId());
}

void Notebook::rebuildDatabaseRecur(const QString &p_folderRelPath, ID p_folderId) {
  const auto cfgPath = joinRel(p_folderRelPath, c_nodeConfigName);
  NodeConfig cfg;
  if (!m_backend->existsFile(cfgPath)) {
    return;
  }
  cfg.fromJson(QJsonDocument::fromJson(m_backend->readFile(cfgPath)).object());

  const ID folderId = (cfg.m_id != 0) ? cfg.m_id : p_folderId;
  NotebookDatabaseAccess::NodeRow folderRow;
  folderRow.m_id = folderId;
  folderRow.m_name = p_folderRelPath.isEmpty()
                         ? m_config.m_name
                         : p_folderRelPath.section(QLatin1Char('/'), -1);
  folderRow.m_signature = cfg.m_signature;
  // parent handled by caller via recursion order; here ensure this folder row exists.
  m_db->addNode(folderRow);

  for (const auto &f : cfg.m_files) {
    NotebookDatabaseAccess::NodeRow row;
    row.m_id = f.m_id;
    row.m_name = f.m_name;
    row.m_signature = f.m_signature;
    row.m_parentId = folderId;
    m_db->addNode(row);
    m_db->removeNodeTags(f.m_id);
    for (const auto &tag : f.m_tags) {
      m_db->addTag(tag, QString());
      m_db->addNodeTag(f.m_id, tag);
    }
  }
  for (const auto &f : cfg.m_folders) {
    const auto childRel = joinRel(p_folderRelPath, f.m_name);
    // Read child's own vx.json to know its id, then insert with parent linkage.
    const auto childCfgPath = joinRel(childRel, c_nodeConfigName);
    ID childId = 0;
    if (m_backend->existsFile(childCfgPath)) {
      NodeConfig childCfg;
      childCfg.fromJson(QJsonDocument::fromJson(m_backend->readFile(childCfgPath)).object());
      childId = childCfg.m_id;
      NotebookDatabaseAccess::NodeRow row;
      row.m_id = childId;
      row.m_name = f.m_name;
      row.m_signature = childCfg.m_signature;
      row.m_parentId = folderId;
      m_db->addNode(row);
    }
    rebuildDatabaseRecur(childRel, childId);
  }
}
