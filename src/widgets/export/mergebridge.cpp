#include "mergebridge.h"

#include <QUrl>

#include <core/export/notemerge.h>

using namespace markly;

MergeBridge::MergeBridge(QObject *p_parent) : QObject(p_parent) {}

bool MergeBridge::exportDir(const QString &p_dirPath, const QString &p_outPath,
                            const QString &p_format) {
  QString dir = p_dirPath;
  if (dir.startsWith(QStringLiteral("file://"))) {
    dir = QUrl(dir).toLocalFile();
  }
  return NoteMerge::exportMergedDir(dir, p_outPath, p_format);
}
