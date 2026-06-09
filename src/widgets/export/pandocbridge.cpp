#include "pandocbridge.h"

#include <core/export/pandocexporter.h>

using namespace markly;

PandocBridge::PandocBridge(QObject *p_parent) : QObject(p_parent) {}

bool PandocBridge::available() const { return PandocExporter::isAvailable(); }

bool PandocBridge::exportNote(const QString &p_markdown, const QString &p_outPath,
                              const QString &p_toFormat) {
  return PandocExporter::exportTo(p_markdown, p_outPath, p_toFormat);
}

bool PandocBridge::exportNoteOpts(const QString &p_markdown, const QString &p_outPath,
                                  const QString &p_toFormat, bool p_standalone, bool p_toc,
                                  const QString &p_cssPath) {
  PandocOptions opts;
  opts.standalone = p_standalone;
  opts.toc = p_toc;
  opts.cssPath = p_cssPath;
  return PandocExporter::exportTo(p_markdown, p_outPath, p_toFormat, opts);
}
