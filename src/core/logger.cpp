#include "logger.h"

#include "configmgr.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QTextStream>

using namespace markly;

QFile Logger::s_file;

bool Logger::s_verbose = false;

bool Logger::s_logToStderr = false;

static QMutex s_logMutex;

void Logger::init(bool p_verbose, bool p_logToStderr) {
  s_verbose = p_verbose;
  s_logToStderr = p_logToStderr;

  if (!s_logToStderr) {
    const auto logFilePath = ConfigMgr::getInst().getLogFile();
    QDir().mkpath(QFileInfo(logFilePath).absolutePath());
    s_file.setFileName(logFilePath);
    // Rotate when too large.
    const auto openMode = (QFileInfo(logFilePath).size() >= 5 * 1024 * 1024)
                              ? (QIODevice::WriteOnly | QIODevice::Text)
                              : (QIODevice::Append | QIODevice::Text);
    if (!s_file.open(openMode)) {
      // Fall back to stderr if the log file is not writable.
      s_logToStderr = true;
      fprintf(stderr, "Warning: failed to open log file %s, logging to stderr\n",
              qPrintable(logFilePath));
    }
  }

  qInstallMessageHandler(Logger::log);
}

static QString getFileName(const char *p_file) {
  if (!p_file) {
    return QString();
  }
  QString file(p_file);
  int idx = file.lastIndexOf(QChar('/'));
  if (idx == -1) {
    idx = file.lastIndexOf(QChar('\\'));
  }

  return idx == -1 ? file : file.mid(idx + 1);
}

void Logger::log(QtMsgType p_type, const QMessageLogContext &p_context, const QString &p_msg) {
  if (!s_verbose && p_type == QtDebugMsg) {
    return;
  }

  QString header;
  switch (p_type) {
  case QtDebugMsg:
    header = QStringLiteral("Debug:");
    break;
  case QtInfoMsg:
    header = QStringLiteral("Info:");
    break;
  case QtWarningMsg:
    header = QStringLiteral("Warning:");
    break;
  case QtCriticalMsg:
    header = QStringLiteral("Critical:");
    break;
  case QtFatalMsg:
    header = QStringLiteral("Fatal:");
    break;
  }

  const QString fileName = getFileName(p_context.file);
  const QString line =
      QStringLiteral("%1(%2:%3) %4").arg(header, fileName).arg(p_context.line).arg(p_msg);

  QMutexLocker locker(&s_logMutex);
  if (!s_logToStderr && s_file.isOpen()) {
    QTextStream stream(&s_file);
    stream << line << "\n";
    stream.flush();
    if (p_type == QtFatalMsg) {
      s_file.close();
      abort();
    }
  } else {
    fprintf(stderr, "%s\n", qPrintable(line));
    fflush(stderr);
    if (p_type == QtFatalMsg) {
      abort();
    }
  }
}
