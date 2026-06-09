#include "fileassoc.h"

#include <QProcess>
#include <QStandardPaths>

using namespace markly;

QStringList FileAssoc::registerCommands() {
  return {QStringLiteral("xdg-mime"), QStringLiteral("default"),
          QStringLiteral("markly.desktop"), QStringLiteral("text/markdown")};
}

bool FileAssoc::registerNow() {
  if (QStandardPaths::findExecutable(QStringLiteral("xdg-mime")).isEmpty()) {
    return false;
  }
  const QStringList cmd = registerCommands();
  QProcess p;
  p.start(cmd.first(), cmd.mid(1));
  if (!p.waitForFinished(10000)) {
    return false;
  }
  return p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0;
}
