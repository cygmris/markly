#include "notebooktagmgr.h"

#include "notebookdatabaseaccess.h"

using namespace markly;

QHash<QString, QString> NotebookTagMgr::parseTagGraph(const QString &p_tagGraph) {
  QHash<QString, QString> parents;
  const auto pairs = p_tagGraph.split(QLatin1Char(';'), Qt::SkipEmptyParts);
  for (const auto &pair : pairs) {
    const auto parts = pair.split(QLatin1Char('>'));
    if (parts.size() == 2) {
      parents.insert(parts.at(1).trimmed(), parts.at(0).trimmed());
    }
  }
  return parents;
}

void NotebookTagMgr::load(NotebookDatabaseAccess *p_db, const QString &p_tagGraph) {
  m_tags = p_db ? p_db->queryAllTags() : QStringList();
  m_parents = parseTagGraph(p_tagGraph);
  // Ensure every parent-linked tag is present in the list.
  for (auto it = m_parents.constBegin(); it != m_parents.constEnd(); ++it) {
    if (!m_tags.contains(it.key())) {
      m_tags.append(it.key());
    }
  }
}

void NotebookTagMgr::addTag(NotebookDatabaseAccess *p_db, const QString &p_name,
                            const QString &p_parent) {
  if (p_name.isEmpty()) {
    return;
  }
  if (p_db) {
    p_db->addTag(p_name, p_parent);
  }
  if (!m_tags.contains(p_name)) {
    m_tags.append(p_name);
  }
  if (p_parent.isEmpty()) {
    m_parents.remove(p_name);
  } else {
    m_parents.insert(p_name, p_parent);
  }
}

void NotebookTagMgr::removeTag(NotebookDatabaseAccess *p_db, const QString &p_name) {
  if (p_db) {
    p_db->removeTag(p_name);
  }
  m_tags.removeAll(p_name);
  m_parents.remove(p_name);
  // Re-parent orphaned children to top level.
  for (auto it = m_parents.begin(); it != m_parents.end();) {
    if (it.value() == p_name) {
      it = m_parents.erase(it);
    } else {
      ++it;
    }
  }
}

QString NotebookTagMgr::toTagGraph() const {
  QStringList pairs;
  for (auto it = m_parents.constBegin(); it != m_parents.constEnd(); ++it) {
    if (!it.value().isEmpty()) {
      pairs.append(it.value() + QLatin1Char('>') + it.key());
    }
  }
  return pairs.join(QLatin1Char(';'));
}
