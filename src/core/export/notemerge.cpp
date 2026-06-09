#include "notemerge.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "pandocexporter.h"

using namespace markly;

QString NoteMerge::merge(const QVector<MergeNote> &p_notes, bool p_includeTitles) {
  QStringList parts;
  for (const MergeNote &n : p_notes) {
    QString s;
    if (p_includeTitles && !n.title.isEmpty()) {
      s += QStringLiteral("# ") + n.title + QStringLiteral("\n\n");
    }
    s += n.content;
    parts << s;
  }
  return parts.join(QStringLiteral("\n\n---\n\n"));
}

QString NoteMerge::mergeDir(const QString &p_dirPath) {
  QDir dir(p_dirPath);
  const QStringList files = dir.entryList({QStringLiteral("*.md")}, QDir::Files, QDir::Name);
  QVector<MergeNote> notes;
  for (const QString &f : files) {
    QFile file(dir.filePath(f));
    if (!file.open(QIODevice::ReadOnly)) {
      continue;
    }
    MergeNote n;
    n.title = QFileInfo(f).completeBaseName();
    n.content = QString::fromUtf8(file.readAll());
    file.close();
    notes.append(n);
  }
  if (notes.isEmpty()) {
    return QString();
  }
  return merge(notes, true);
}

bool NoteMerge::exportMergedDir(const QString &p_dirPath, const QString &p_outPath,
                                const QString &p_format, QString *p_err) {
  const QString md = mergeDir(p_dirPath);
  if (md.isEmpty()) {
    if (p_err) *p_err = QStringLiteral("no .md files to merge");
    return false;
  }
  if (p_format == QStringLiteral("md")) {
    QFile out(p_outPath);
    if (!out.open(QIODevice::WriteOnly)) {
      if (p_err) *p_err = QStringLiteral("cannot write %1").arg(p_outPath);
      return false;
    }
    out.write(md.toUtf8());
    out.close();
    return true;
  }
  return PandocExporter::exportTo(md, p_outPath, p_format, p_err);
}
