#ifndef MARKLY_NOTEMERGE_H
#define MARKLY_NOTEMERGE_H

#include <QString>
#include <QVector>

namespace markly {
struct MergeNote {
  QString title;
  QString content;
};

// Merges multiple Markdown notes into one document, then exports (#15c). The merge
// logic is pure (unit-testable); exportMergedDir reuses PandocExporter (#15b).
class NoteMerge {
public:
  // Join notes with "\n\n---\n\n"; optionally prefix each with "# <title>".
  static QString merge(const QVector<MergeNote> &p_notes, bool p_includeTitles = true);
  // Read *.md in dirPath (sorted by name), title = file base name, then merge. "" if none.
  static QString mergeDir(const QString &p_dirPath);
  // Merge dirPath then write (format=="md") or convert via pandoc. Returns success.
  static bool exportMergedDir(const QString &p_dirPath, const QString &p_outPath,
                              const QString &p_format, QString *p_err = nullptr);
};
} // namespace markly

#endif // MARKLY_NOTEMERGE_H
