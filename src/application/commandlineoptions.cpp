#include "commandlineoptions.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>

using namespace markly;

CommandLineOptions::ParseResult CommandLineOptions::parse(const QStringList &p_arguments) {
  QCommandLineParser parser;
  parser.setApplicationDescription(tr("A pleasant note-taking platform."));
  const auto helpOpt = parser.addHelpOption();
  const auto versionOpt = parser.addVersionOption();

  parser.addPositionalArgument("paths", tr("Files or folders to open."));

  const QCommandLineOption verboseOpt("verbose", tr("Print more logs."));
  parser.addOption(verboseOpt);

  const QCommandLineOption logStderrOpt("log-stderr", tr("Log to stderr."));
  parser.addOption(logStderrOpt);

  const QCommandLineOption watchThemesOpt("watch-themes",
                                          tr("Watch theme folder for changes."));
  parser.addOption(watchThemesOpt);

  // WebEngine options. No need to handle; add to avoid parse error.
  {
    QCommandLineOption webRemoteDebuggingPortOpt("remote-debugging-port",
                                                 tr("WebEngine remote debugging port."),
                                                 tr("port_number"));
    webRemoteDebuggingPortOpt.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(webRemoteDebuggingPortOpt);

    QCommandLineOption webNoSandboxOpt("no-sandbox", tr("WebEngine without sandbox."));
    webNoSandboxOpt.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(webNoSandboxOpt);

    QCommandLineOption webDisableGpu("disable-gpu", tr("WebEngine with GPU disabled."));
    webDisableGpu.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(webDisableGpu);
  }

  if (!parser.parse(p_arguments)) {
    m_errorMsg = parser.errorText();
    return ParseResult::Error;
  }

  m_helpText = parser.helpText();
  if (parser.isSet(helpOpt)) {
    return ParseResult::HelpRequested;
  }

  if (parser.isSet(versionOpt)) {
    return ParseResult::VersionRequested;
  }

  m_pathsToOpen = parser.positionalArguments();

  if (parser.isSet(verboseOpt)) {
    m_verbose = true;
  }

  if (parser.isSet(logStderrOpt)) {
    m_logToStderr = true;
  }

  if (parser.isSet(watchThemesOpt)) {
    m_watchThemes = true;
  }

  return ParseResult::Ok;
}
