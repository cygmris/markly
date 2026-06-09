#include "pandocexporter.h"

#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryFile>

using namespace markly;

QStringList PandocExporter::buildArgs(const QString &p_inPath, const QString &p_outPath,
                                      const QString &p_toFormat, const QString &p_fromFormat) {
  return {QStringLiteral("-f"), p_fromFormat, QStringLiteral("-t"), p_toFormat,
          QStringLiteral("-o"), p_outPath,    p_inPath};
}

QStringList PandocExporter::buildArgs(const QString &p_inPath, const QString &p_outPath,
                                      const QString &p_toFormat, const QString &p_fromFormat,
                                      const PandocOptions &p_opts) {
  QStringList args = buildArgs(p_inPath, p_outPath, p_toFormat, p_fromFormat);
  if (p_opts.standalone) {
    args << QStringLiteral("--standalone");
  }
  if (p_opts.toc) {
    args << QStringLiteral("--toc");
  }
  if (!p_opts.cssPath.isEmpty()) {
    args << QStringLiteral("--css") << p_opts.cssPath;
  }
  return args;
}

bool PandocExporter::isAvailable() {
  return !QStandardPaths::findExecutable(QStringLiteral("pandoc")).isEmpty();
}

bool PandocExporter::exportTo(const QString &p_markdown, const QString &p_outPath,
                              const QString &p_toFormat, QString *p_err) {
  return exportTo(p_markdown, p_outPath, p_toFormat, PandocOptions{}, p_err);
}

bool PandocExporter::exportTo(const QString &p_markdown, const QString &p_outPath,
                              const QString &p_toFormat, const PandocOptions &p_opts,
                              QString *p_err) {
  auto fail = [p_err](const QString &m) {
    if (p_err) *p_err = m;
    return false;
  };
  if (!isAvailable()) {
    return fail(QStringLiteral("pandoc not found"));
  }

  QTemporaryFile in(QDir::tempPath() + QStringLiteral("/markly-XXXXXX.md"));
  if (!in.open()) {
    return fail(QStringLiteral("cannot create temp input file"));
  }
  in.write(p_markdown.toUtf8());
  in.flush();

  QProcess p;
  p.start(QStringLiteral("pandoc"),
          buildArgs(in.fileName(), p_outPath, p_toFormat, QStringLiteral("markdown"), p_opts));
  if (!p.waitForFinished(30000)) {
    return fail(QStringLiteral("pandoc timed out"));
  }
  if (p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0) {
    return fail(QString::fromUtf8(p.readAllStandardError()));
  }
  if (QFileInfo(p_outPath).size() <= 0) {
    return fail(QStringLiteral("pandoc produced an empty file"));
  }
  return true;
}
