#include "snippetbridge.h"

#include <core/snippet/snippetmgr.h>

using namespace markly;

SnippetBridge::SnippetBridge(SnippetMgr *p_mgr, QObject *p_parent)
    : QObject(p_parent), m_mgr(p_mgr) {
  if (m_mgr) {
    connect(m_mgr, &SnippetMgr::changed, this, &SnippetBridge::changed);
  }
}

QVariantList SnippetBridge::list() const {
  QVariantList out;
  if (!m_mgr) {
    return out;
  }
  for (const auto &s : m_mgr->snippets()) {
    QVariantMap m;
    m[QStringLiteral("name")] = s.m_name;
    m[QStringLiteral("description")] = s.m_description;
    out.append(m);
  }
  return out;
}

QVariantMap SnippetBridge::apply(const QString &p_name, const QString &p_selectedText,
                                 const QString &p_noteName) {
  QVariantMap result;
  result[QStringLiteral("text")] = QString();
  result[QStringLiteral("cursorOffset")] = 0;
  if (!m_mgr) {
    return result;
  }
  int offset = 0;
  const auto text = m_mgr->apply(p_name, p_selectedText, p_noteName, offset);
  result[QStringLiteral("text")] = text;
  result[QStringLiteral("cursorOffset")] = offset;
  return result;
}

void SnippetBridge::add(const QString &p_name, const QString &p_content) {
  if (m_mgr) {
    m_mgr->add(p_name, p_content);
  }
}

void SnippetBridge::remove(const QString &p_name) {
  if (m_mgr) {
    m_mgr->remove(p_name);
  }
}
