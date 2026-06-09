#ifndef MARKLY_PANDOCBRIDGE_H
#define MARKLY_PANDOCBRIDGE_H

#include <QObject>
#include <QString>

namespace markly {
// QML bridge (context property "Pandoc", #15b) for Pandoc custom-format export.
class PandocBridge : public QObject {
  Q_OBJECT
public:
  explicit PandocBridge(QObject *p_parent = nullptr);

  Q_INVOKABLE bool available() const;
  // Export markdown to outPath in toFormat (docx/epub/latex/...). Returns success.
  Q_INVOKABLE bool exportNote(const QString &p_markdown, const QString &p_outPath,
                              const QString &p_toFormat);
  // With export presets (#15d): standalone document, table of contents, custom CSS.
  Q_INVOKABLE bool exportNoteOpts(const QString &p_markdown, const QString &p_outPath,
                                  const QString &p_toFormat, bool p_standalone, bool p_toc,
                                  const QString &p_cssPath);
};
} // namespace markly

#endif // MARKLY_PANDOCBRIDGE_H
