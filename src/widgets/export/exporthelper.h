#ifndef MARKLY_EXPORTHELPER_H
#define MARKLY_EXPORTHELPER_H

#include <QObject>
#include <QString>

namespace markly {
// Writes exported text (Markdown / rendered HTML) to disk. PDF is produced by the
// QML ExportView via QWebEngine printToPdf. Context property "Export". markly_core.
class ExportHelper : public QObject {
  Q_OBJECT
public:
  explicit ExportHelper(QObject *p_parent = nullptr);

  // Write UTF-8 text to path; returns true on success.
  Q_INVOKABLE bool writeText(const QString &p_path, const QString &p_text) const;
  // Read a bundled resource (e.g. ":/data/web/preview.css") as UTF-8 text.
  Q_INVOKABLE QString readResource(const QString &p_path) const;
};
} // namespace markly

#endif // MARKLY_EXPORTHELPER_H
