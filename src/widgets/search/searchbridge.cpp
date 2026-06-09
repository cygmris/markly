#include "searchbridge.h"

#include <QVariantMap>

#include <core/marklyapp.h>
#include <core/notebook/node.h>
#include <core/notebook/notebook.h>
#include <core/notebook/notebookdatabaseaccess.h>
#include <core/notebookmgr.h>
#include <core/search/searcher.h>

using namespace markly;

SearchBridge::SearchBridge(NotebookMgr *p_notebookMgr, QObject *p_parent)
    : QObject(p_parent), m_notebookMgr(p_notebookMgr),
      m_searcher(new Searcher(p_notebookMgr)) {
  connect(&MarklyApp::getInst(), &MarklyApp::noteSaved, this, &SearchBridge::onNoteSaved);
}

SearchBridge::~SearchBridge() = default;

void SearchBridge::setState(const QString &p_state) {
  if (m_state != p_state) {
    m_state = p_state;
    emit stateChanged();
  }
}

void SearchBridge::search(const QString &p_keyword, int p_scope, int p_objects,
                          const QString &p_folderRelPath) {
  m_keyword = p_keyword;
  setState(QStringLiteral("busy"));

  SearchOption opt;
  opt.m_keyword = p_keyword;
  opt.m_scope = static_cast<SearchScope>(p_scope);
  opt.m_objects = SearchObjects(p_objects ? p_objects : (ObjName | ObjContent));
  opt.m_folderRelPath = p_folderRelPath;

  m_items = m_searcher->search(opt);

  m_results.clear();
  for (const auto &item : m_items) {
    QVariantMap m;
    m[QStringLiteral("name")] = item.m_name;
    m[QStringLiteral("path")] = item.m_path;
    m[QStringLiteral("snippet")] = item.m_snippet;
    m[QStringLiteral("line")] = item.m_line;
    m_results.append(m);
  }
  emit resultsChanged();
  setState(QStringLiteral("finished"));
}

void SearchBridge::openResult(int p_index) {
  if (p_index < 0 || p_index >= m_items.size()) {
    return;
  }
  const auto &item = m_items.at(p_index);
  // Set the pending jump first so the editor picks it up when the buffer loads.
  if (item.m_line >= 1) {
    emit MarklyApp::getInst().gotoLineRequested(item.m_line);
  }
  emit MarklyApp::getInst().openFileRequested(item.m_path);
}

void SearchBridge::clear() {
  m_items.clear();
  m_results.clear();
  m_keyword.clear();
  emit resultsChanged();
  setState(QStringLiteral("idle"));
}

void SearchBridge::onNoteSaved(const QString &p_filePath) {
  if (!m_notebookMgr) {
    return;
  }
  // Find the saved note's notebook + node and re-index it — but only if that notebook's
  // index already exists (non-empty), so we don't trigger a full build on every save.
  for (const auto &nb : m_notebookMgr->getNotebooks()) {
    if (!nb || !p_filePath.startsWith(nb->getRootPath())) {
      continue;
    }
    auto *db = nb->getDatabase();
    if (!db || !db->isOpen() || db->ftsIsEmpty()) {
      continue;
    }
    const auto nodes = nb->collectMarkdownNodes();
    for (const auto &node : nodes) {
      if (node->fetchAbsolutePath() == p_filePath) {
        m_searcher->reindexNode(nb.data(), node);
        return;
      }
    }
  }
}
