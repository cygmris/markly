#ifndef NOTEBOOKDATABASEACCESS_H
#define NOTEBOOKDATABASEACCESS_H

#include <QPair>
#include <QString>
#include <QStringList>
#include <QVector>

#include <core/global.h>

namespace markly {
// SQLite index for a notebook (node/tag/tag_node). The vx.json files are the
// source of truth; this DB is a queryable index. Schema is VNote-compatible.
class NotebookDatabaseAccess {
public:
  struct NodeRow {
    ID m_id = 0;
    QString m_name;
    ID m_signature = 0;
    ID m_parentId = 0; // 0 == NULL (root)
  };

  NotebookDatabaseAccess(const QString &p_dbPath, const QString &p_connectionName);
  ~NotebookDatabaseAccess();

  bool open();
  void close();

  bool createTables();

  // node table.
  // Insert a new node WITHOUT a fixed id and return the DB-generated id (0 on failure).
  ID insertNode(const QString &p_name, ID p_signature, ID p_parentId);
  // Insert/replace a node with an explicit id (used when rebuilding from vx.json).
  bool addNode(const NodeRow &p_row);
  bool updateNode(const NodeRow &p_row);
  bool removeNode(ID p_id);
  bool nodeExists(ID p_id) const;
  QVector<NodeRow> queryChildren(ID p_parentId) const;

  // tag table.
  bool addTag(const QString &p_name, const QString &p_parentName);
  bool removeTag(const QString &p_name);
  QStringList queryAllTags() const;

  // tag_node table.
  bool addNodeTag(ID p_nodeId, const QString &p_tagName);
  bool removeNodeTags(ID p_nodeId);
  QStringList queryNodeTags(ID p_nodeId) const;
  // Node ids carrying a given tag (exact tag name match).
  QVector<ID> queryNodesByTag(const QString &p_tagName) const;

  // Full-text search index (FTS5 virtual table node_fts). The vx.json/markdown
  // files remain the source of truth; this is a queryable content index.
  bool ensureFtsTable();
  bool ftsIsEmpty() const;
  // Upsert (delete-then-insert) the indexed row for a node.
  bool ftsUpsert(ID p_nodeId, const QString &p_name, const QString &p_path,
                 const QString &p_content);
  bool ftsRemove(ID p_nodeId);
  // Content MATCH: returns (node_id, snippet) ordered by rank.
  QVector<QPair<ID, QString>> ftsQueryContent(const QString &p_ftsExpr) const;

  bool isOpen() const { return m_open; }

private:
  QString m_dbPath;
  QString m_connectionName;
  bool m_open = false;
};
} // namespace markly

#endif // NOTEBOOKDATABASEACCESS_H
