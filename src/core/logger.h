#ifndef LOGGER_H
#define LOGGER_H

#include <QMessageLogContext>
#include <QString>

class QFile;

namespace markly {
class Logger {
public:
  Logger() = delete;

  // @p_verbose: also emit debug-level messages.
  // @p_logToStderr: write to stderr instead of the log file.
  static void init(bool p_verbose, bool p_logToStderr);

private:
  static void log(QtMsgType p_type, const QMessageLogContext &p_context, const QString &p_msg);

  static QFile s_file;

  static bool s_verbose;

  static bool s_logToStderr;
};
} // namespace markly

#endif // LOGGER_H
