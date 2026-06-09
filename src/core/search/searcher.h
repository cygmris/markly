#ifndef MARKLY_SEARCHER_H
#define MARKLY_SEARCHER_H

#include <QSharedPointer>
#include <QVector>

#include <core/search/searchdata.h>

namespace markly {
class NotebookMgr;
class Notebook;
class Node;

// Full-text searcher over the open notebooks. Content is matched via the per-notebook
// FTS5 index (built lazily on first use); name/path are matched in memory; tags via the
// tag_node table. The vx.json/markdown files remain the source of truth.
class Searcher {
public:
  explicit Searcher(NotebookMgr *p_notebookMgr);

  QVector<SearchResultItem> search(const SearchOption &p_option);

  // Re-index a single node's content (used for incremental updates on save).
  void reindexNode(Notebook *p_notebook, const QSharedPointer<Node> &p_node);

private:
  // Ensure the notebook's FTS table exists and is populated (full build if empty).
  void ensureIndex(Notebook *p_notebook);

  QVector<SearchResultItem> searchNotebook(Notebook *p_notebook, const SearchOption &p_option);

  static QString readNodeContent(const QSharedPointer<Node> &p_node);

  NotebookMgr *m_notebookMgr = nullptr;
};
} // namespace markly

#endif // MARKLY_SEARCHER_H
