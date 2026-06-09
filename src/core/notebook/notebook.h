#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#include <QScopedPointer>
#include <QSharedPointer>
#include <QString>

#include <core/global.h>
#include <core/noncopyable.h>

#include "../notebookconfigmgr/notebookconfig.h"
#include "../notebookconfigmgr/vxnodeconfig.h"
#include "../versioncontroller/iversioncontroller.h"
#include "node.h"
#include "notebooktagmgr.h"

namespace markly {
class INotebookBackend;
class NotebookDatabaseAccess;

// A bundle notebook: a root directory holding vx_notebook.json + per-folder vx.json,
// indexed by a SQLite db. vx.json is the source of truth; the db is a queryable
// index. Disk format is VNote v3.x compatible.
// (VNote's Notebook/BundleNotebook + INotebookConfigMgr/VXNotebookConfigMgr are
// merged here, since Markly has a single format + local backend.)
class Notebook : private Noncopyable {
public:
  // Build a notebook bound to an existing root that already has vx_notebook.json.
  // Use NotebookFactory to construct/open/create.
  Notebook(ID p_id, const QString &p_rootPath, INotebookBackend *p_backend);
  ~Notebook();

  // Open: read vx_notebook.json, open/build db index, load the root node.
  void open();

  // Create a brand-new notebook on disk (vx_notebook.json + root vx.json + db).
  void createOnDisk(const QString &p_name, const QString &p_description);

  ID getId() const { return m_id; }
  QString getName() const { return m_config.m_name; }
  QString getDescription() const { return m_config.m_description; }
  const QString &getRootPath() const { return m_rootPath; }
  const NotebookConfig &getConfig() const { return m_config; }

  INotebookBackend *getBackend() const { return m_backend.data(); }
  NotebookDatabaseAccess *getDatabase() const { return m_db.data(); }
  NotebookTagMgr *getTagMgr() const { return m_tagMgr.data(); }
  IVersionController *getVersionController() const { return m_vc.data(); }

  QSharedPointer<Node> getRootNode() const { return m_root; }

  // Lazily load a folder node's children from its vx.json (no-op if loaded).
  void loadNodeChildren(const QSharedPointer<Node> &p_node);

  // Recursively load and collect all markdown file nodes (for full-text indexing).
  QVector<QSharedPointer<Node>> collectMarkdownNodes();

  // CRUD. Each writes disk (truth) then syncs the db index.
  QSharedPointer<Node> newNode(const QSharedPointer<Node> &p_parent, Node::Type p_type,
                               const QString &p_name);
  // Register an existing on-disk file/folder (external) as a managed node.
  // Does not overwrite its content. Returns null if it does not exist.
  QSharedPointer<Node> importNode(const QSharedPointer<Node> &p_parent, Node::Type p_type,
                                  const QString &p_name);
  void renameNode(const QSharedPointer<Node> &p_node, const QString &p_newName);
  void removeNode(const QSharedPointer<Node> &p_node); // permanent
  void moveNodeToRecycleBin(const QSharedPointer<Node> &p_node);
  void updateNodeVisual(const QSharedPointer<Node> &p_node, const NodeVisual &p_visual);
  void updateNodeTags(const QSharedPointer<Node> &p_node, const QStringList &p_tags);

  // Persist vx_notebook.json.
  void saveConfig();

  static const QString c_notebookConfigName; // vx_notebook.json
  static const QString c_nodeConfigName;     // vx.json
  static const QString c_recycleBinFolder;   // vx_recycle_bin
  static const QString c_databaseName;       // vx_notebook.db

private:
  // vx.json relative path for a folder node ("" => root).
  QString nodeConfigRelPath(const Node *p_folder) const;

  // Read/write a folder node's vx.json (raw pointer so parent pointers work too).
  vx_node_config::NodeConfig readNodeConfig(const Node *p_folder) const;
  void writeNodeConfig(Node *p_folder);

  void collectMarkdownNodes(const QSharedPointer<Node> &p_node,
                            QVector<QSharedPointer<Node>> &p_out);

  // Rebuild the db index by walking the vx.json tree on disk.
  void rebuildDatabase();
  void rebuildDatabaseRecur(const QString &p_folderRelPath, ID p_folderId);

  // The folder that contains @p_node on disk (its parent for files; itself for folders'
  // own config is its own vx.json, but membership is recorded in the parent's vx.json).
  void ensureLoaded(const QSharedPointer<Node> &p_node);

  ID m_id;
  QString m_rootPath;
  QScopedPointer<INotebookBackend> m_backend;
  QScopedPointer<NotebookDatabaseAccess> m_db;
  NotebookConfig m_config;
  QSharedPointer<Node> m_root;
  QScopedPointer<NotebookTagMgr> m_tagMgr;
  QScopedPointer<IVersionController> m_vc;
};
} // namespace markly

#endif // NOTEBOOK_H
