#ifndef MARKLY_SEARCHDATA_H
#define MARKLY_SEARCHDATA_H

#include <QFlags>
#include <QRegularExpression>
#include <QString>
#include <QStringList>

#include <core/global.h>

namespace markly {
// Where to search.
enum class SearchScope { CurrentNotebook = 0, AllNotebooks, CurrentFolder, OpenBuffers };

// What to match (combinable).
enum SearchObject {
  ObjNone = 0x0,
  ObjName = 0x1,
  ObjContent = 0x2,
  ObjPath = 0x4,
  ObjTag = 0x8,
};
Q_DECLARE_FLAGS(SearchObjects, SearchObject)

struct SearchOption {
  QString m_keyword;
  SearchScope m_scope = SearchScope::CurrentNotebook;
  SearchObjects m_objects = SearchObjects(ObjName | ObjContent);
  QString m_folderRelPath;   // used when scope == CurrentFolder
  QStringList m_bufferPaths; // used when scope == OpenBuffers

  // Build an FTS5 MATCH expression from the keyword: split on whitespace, wrap
  // each token as a quoted phrase (so special characters are taken literally and
  // FTS5 syntax errors are avoided), join with spaces (FTS5 defaults to AND).
  static QString toFtsExpr(const QString &p_keyword) {
    const auto tokens = p_keyword.split(QRegularExpression(QStringLiteral("\\s+")),
                                        Qt::SkipEmptyParts);
    QStringList phrases;
    for (auto tok : tokens) {
      tok.replace(QLatin1Char('"'), QStringLiteral("\"\"")); // escape inner quotes
      phrases.append(QLatin1Char('"') + tok + QLatin1Char('"'));
    }
    return phrases.join(QLatin1Char(' '));
  }
};

struct SearchResultItem {
  ID m_notebookId = 0;
  ID m_nodeId = 0;
  QString m_name;
  QString m_path;    // absolute path on disk
  QString m_snippet; // excerpt with <mark>..</mark> around the hit
  int m_line = -1;   // optional line to jump to
};
} // namespace markly

Q_DECLARE_OPERATORS_FOR_FLAGS(markly::SearchObjects)

#endif // MARKLY_SEARCHDATA_H
