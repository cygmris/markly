#ifndef MARKLY_PANDOCEXPORTER_H
#define MARKLY_PANDOCEXPORTER_H

#include <QString>
#include <QStringList>

namespace markly {
// Export presets (#15d): standalone document, table of contents, custom CSS.
struct PandocOptions {
  bool standalone = false;
  bool toc = false;
  QString cssPath;
};

// Exports Markdown to docx / epub / latex / ... via the system `pandoc` (#15b/#15d).
// buildArgs / isAvailable are pure (unit-testable); exportTo runs pandoc via QProcess.
class PandocExporter {
public:
  static QStringList buildArgs(const QString &p_inPath, const QString &p_outPath,
                               const QString &p_toFormat,
                               const QString &p_fromFormat = QStringLiteral("markdown"));
  // With export options (#15d): appends --standalone / --toc / --css.
  static QStringList buildArgs(const QString &p_inPath, const QString &p_outPath,
                               const QString &p_toFormat, const QString &p_fromFormat,
                               const PandocOptions &p_opts);
  static bool isAvailable();

  // Write `p_markdown` to a temp file, run pandoc to `p_outPath` in `p_toFormat`.
  // Returns true on exit-code 0 with a non-empty output; *p_err carries the reason on failure.
  static bool exportTo(const QString &p_markdown, const QString &p_outPath,
                       const QString &p_toFormat, QString *p_err = nullptr);
  static bool exportTo(const QString &p_markdown, const QString &p_outPath,
                       const QString &p_toFormat, const PandocOptions &p_opts,
                       QString *p_err = nullptr);
};
} // namespace markly

#endif // MARKLY_PANDOCEXPORTER_H
