#ifndef NOTEBOOKTAGMGR_H
#define NOTEBOOKTAGMGR_H

#include <QHash>
#include <QString>
#include <QStringList>

namespace markly {
class NotebookDatabaseAccess;

// In-memory tag hierarchy for a notebook. Tags live in the SQLite `tag` table;
// the parent relationships are also serialized to vx_notebook.json's tag_graph
// ("parent>child;parent2>child2"). Ported (trimmed) from VNote NotebookTagMgr.
class NotebookTagMgr {
public:
  // Load tags from the db and parent links from @p_tagGraph.
  void load(NotebookDatabaseAccess *p_db, const QString &p_tagGraph);

  QStringList getTags() const { return m_tags; }

  // Empty string == top-level tag.
  QString getParent(const QString &p_tag) const { return m_parents.value(p_tag); }

  void addTag(NotebookDatabaseAccess *p_db, const QString &p_name, const QString &p_parent);
  void removeTag(NotebookDatabaseAccess *p_db, const QString &p_name);

  // Serialize parent links back to the tag_graph string.
  QString toTagGraph() const;

  static QHash<QString, QString> parseTagGraph(const QString &p_tagGraph);

private:
  QStringList m_tags;
  QHash<QString, QString> m_parents; // tag -> parent ("" = top)
};
} // namespace markly

#endif // NOTEBOOKTAGMGR_H
