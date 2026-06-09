#include "searcher.h"

#include <QFile>
#include <QHash>
#include <QSet>

#include <core/notebook/node.h>
#include <core/notebook/notebook.h>
#include <core/notebook/notebookdatabaseaccess.h>
#include <core/notebookmgr.h>

using namespace markly;

Searcher::Searcher(NotebookMgr *p_notebookMgr) : m_notebookMgr(p_notebookMgr) {}

QString Searcher::readNodeContent(const QSharedPointer<Node> &p_node) {
  QFile file(p_node->fetchAbsolutePath());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "search: cannot read" << p_node->fetchAbsolutePath();
    return QString();
  }
  return QString::fromUtf8(file.readAll());
}

void Searcher::ensureIndex(Notebook *p_notebook) {
  auto *db = p_notebook->getDatabase();
  if (!db || !db->isOpen()) {
    return;
  }
  db->ensureFtsTable();
  if (!db->ftsIsEmpty()) {
    return;
  }
  // Full build: walk every markdown node, read its content, index it.
  const auto nodes = p_notebook->collectMarkdownNodes();
  for (const auto &node : nodes) {
    db->ftsUpsert(node->getId(), node->getName(), node->fetchRelativePath(),
                  readNodeContent(node));
  }
}

void Searcher::reindexNode(Notebook *p_notebook, const QSharedPointer<Node> &p_node) {
  auto *db = p_notebook->getDatabase();
  if (!db || !db->isOpen() || !p_node || p_node->isContainer()) {
    return;
  }
  db->ensureFtsTable();
  db->ftsUpsert(p_node->getId(), p_node->getName(), p_node->fetchRelativePath(),
                readNodeContent(p_node));
}

QVector<SearchResultItem> Searcher::searchNotebook(Notebook *p_notebook,
                                                   const SearchOption &p_option) {
  QVector<SearchResultItem> results;
  auto *db = p_notebook->getDatabase();
  if (!db || !db->isOpen()) {
    return results;
  }
  ensureIndex(p_notebook);

  // Node id -> node, for resolving hits to name/path and for in-memory matching.
  const auto nodes = p_notebook->collectMarkdownNodes();
  QHash<ID, QSharedPointer<Node>> byId;
  for (const auto &n : nodes) {
    byId.insert(n->getId(), n);
  }

  // Scope filter predicate over a node.
  const auto inScope = [&](const QSharedPointer<Node> &n) -> bool {
    switch (p_option.m_scope) {
    case SearchScope::CurrentFolder:
      if (p_option.m_folderRelPath.isEmpty()) {
        return true;
      }
      return n->fetchRelativePath().startsWith(p_option.m_folderRelPath + QLatin1Char('/')) ||
             n->fetchRelativePath() == p_option.m_folderRelPath;
    case SearchScope::OpenBuffers:
      return p_option.m_bufferPaths.contains(n->fetchAbsolutePath());
    default:
      return true;
    }
  };

  const QString kw = p_option.m_keyword;

  // First 1-based line of `node` that contains any keyword token (case-insensitive),
  // or -1. Used so search results can jump to the hit.
  const auto firstHitLine = [&](const QSharedPointer<Node> &n) -> int {
    const auto tokens = kw.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    if (tokens.isEmpty()) {
      return -1;
    }
    const auto lines = readNodeContent(n).split(QLatin1Char('\n'));
    for (int i = 0; i < lines.size(); ++i) {
      for (const auto &tok : tokens) {
        if (lines.at(i).contains(tok, Qt::CaseInsensitive)) {
          return i + 1;
        }
      }
    }
    return -1;
  };

  QSet<ID> seen;
  const auto addResult = [&](const QSharedPointer<Node> &n, const QString &snippet, int line) {
    if (!n || seen.contains(n->getId()) || !inScope(n)) {
      return;
    }
    seen.insert(n->getId());
    SearchResultItem item;
    item.m_notebookId = p_notebook->getId();
    item.m_nodeId = n->getId();
    item.m_name = n->getName();
    item.m_path = n->fetchAbsolutePath();
    item.m_snippet = snippet;
    item.m_line = line;
    results.append(item);
  };

  // Content: FTS5 MATCH.
  if (p_option.m_objects.testFlag(ObjContent)) {
    const auto hits = db->ftsQueryContent(SearchOption::toFtsExpr(kw));
    for (const auto &hit : hits) {
      auto node = byId.value(hit.first);
      addResult(node, hit.second, node ? firstHitLine(node) : -1);
    }
  }

  // Name / Path: in-memory substring match (case-insensitive).
  if (p_option.m_objects.testFlag(ObjName) || p_option.m_objects.testFlag(ObjPath)) {
    for (const auto &n : nodes) {
      bool match = false;
      if (p_option.m_objects.testFlag(ObjName) &&
          n->getName().contains(kw, Qt::CaseInsensitive)) {
        match = true;
      }
      if (!match && p_option.m_objects.testFlag(ObjPath) &&
          n->fetchRelativePath().contains(kw, Qt::CaseInsensitive)) {
        match = true;
      }
      if (match) {
        addResult(n, n->fetchRelativePath(), -1);
      }
    }
  }

  // Tag: nodes whose tags contain the keyword (substring, case-insensitive).
  if (p_option.m_objects.testFlag(ObjTag)) {
    for (const auto &n : nodes) {
      for (const auto &tag : n->getTags()) {
        if (tag.contains(kw, Qt::CaseInsensitive)) {
          addResult(n, QStringLiteral("#") + tag, -1);
          break;
        }
      }
    }
  }

  return results;
}

QVector<SearchResultItem> Searcher::search(const SearchOption &p_option) {
  QVector<SearchResultItem> results;
  if (p_option.m_keyword.trimmed().isEmpty() || !m_notebookMgr) {
    return results;
  }

  QVector<QSharedPointer<Notebook>> targets;
  switch (p_option.m_scope) {
  case SearchScope::AllNotebooks:
    targets = m_notebookMgr->getNotebooks();
    break;
  case SearchScope::CurrentNotebook:
  case SearchScope::CurrentFolder:
  case SearchScope::OpenBuffers:
  default:
    if (auto cur = m_notebookMgr->getCurrentNotebook()) {
      targets.append(cur);
    }
    // OpenBuffers may span notebooks; include all so cross-notebook buffers match.
    if (p_option.m_scope == SearchScope::OpenBuffers) {
      targets = m_notebookMgr->getNotebooks();
    }
    break;
  }

  for (const auto &nb : targets) {
    if (nb) {
      results += searchNotebook(nb.data(), p_option);
    }
  }
  return results;
}
