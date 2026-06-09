#include "exporthelper.h"

#include <QFile>

using namespace markly;

ExportHelper::ExportHelper(QObject *p_parent) : QObject(p_parent) {}

bool ExportHelper::writeText(const QString &p_path, const QString &p_text) const {
  if (p_path.isEmpty()) {
    return false;
  }
  QFile f(p_path);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return false;
  }
  const bool ok = f.write(p_text.toUtf8()) >= 0;
  f.close();
  return ok;
}

QString ExportHelper::readResource(const QString &p_path) const {
  QFile f(p_path);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return QString();
  }
  return QString::fromUtf8(f.readAll());
}
