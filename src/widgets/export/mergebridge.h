#ifndef MARKLY_MERGEBRIDGE_H
#define MARKLY_MERGEBRIDGE_H

#include <QObject>
#include <QString>

namespace markly {
// QML bridge (context property "Merge", #15c) for merged folder export.
class MergeBridge : public QObject {
  Q_OBJECT
public:
  explicit MergeBridge(QObject *p_parent = nullptr);

  // Merge all *.md in dirPath, export to outPath in format (md/html/docx/...).
  Q_INVOKABLE bool exportDir(const QString &p_dirPath, const QString &p_outPath,
                             const QString &p_format);
};
} // namespace markly

#endif // MARKLY_MERGEBRIDGE_H
