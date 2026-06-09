#include <QDateTime>
#include <QIcon>
#include <QSysInfo>

#include <application/application.h>
#include <application/commandlineoptions.h>
#include <core/configmgr.h>
#include <core/exception.h>
#include <core/logger.h>
#include <core/marklyapp.h>
#include <core/singleinstanceguard.h>
#include <core/thememgr.h>
#include <widgets/mainwindow.h>

using namespace markly;

int main(int argc, char *argv[]) {
  QApplication::setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

  Application app(argc, argv);

  ConfigMgr::initAppPrefixPath();

  app.setApplicationName(ConfigMgr::c_appName);
  app.setOrganizationName(ConfigMgr::c_orgName);
  app.setApplicationVersion(ConfigMgr::getApplicationVersion());

  // Parse command line.
  CommandLineOptions cmdOptions;
  switch (cmdOptions.parse(app.arguments())) {
  case CommandLineOptions::Ok:
    break;

  case CommandLineOptions::Error:
    fprintf(stderr, "%s\n", qPrintable(cmdOptions.m_errorMsg));
    return -1;

  case CommandLineOptions::VersionRequested:
    fprintf(stderr, "%s %s\n", qPrintable(app.applicationName()),
            qPrintable(app.applicationVersion()));
    return 0;

  case CommandLineOptions::HelpRequested:
  default:
    fprintf(stderr, "%s\n", qPrintable(cmdOptions.m_helpText));
    return 0;
  }

  // Single instance guard.
  SingleInstanceGuard guard;
  if (!guard.tryRun()) {
    guard.requestOpenFiles(cmdOptions.m_pathsToOpen);
    guard.requestShow();
    return 0;
  }

  // Configuration manager.
  try {
    ConfigMgr::getInst();
  } catch (Exception &e) {
    fprintf(stderr, "%s failed to start: %s\n", qPrintable(ConfigMgr::c_appName), e.what());
    return -1;
  }

  // Logger after app info and config are ready.
  Logger::init(cmdOptions.m_verbose, cmdOptions.m_logToStderr);

  qInfo() << QStringLiteral("%1 (v%2) started at %3 (%4)")
                 .arg(ConfigMgr::c_appName, app.applicationVersion(),
                      QDateTime::currentDateTime().toString(), QSysInfo::productType());

  // Theme stylesheet (empty in the foundation phase).
  {
    auto style = MarklyApp::getInst().getThemeMgr().fetchQtStyleSheet();
    if (!style.isEmpty()) {
      app.setStyleSheet(style);
      if (cmdOptions.m_watchThemes) {
        app.watchThemeFolder(ConfigMgr::getInst().getUserThemeFolder());
      }
    }
  }

  MainWindow window;
  MarklyApp::getInst().setMainWindow(&window);
  window.show();

  QObject::connect(&guard, &SingleInstanceGuard::showRequested, &window,
                   &MainWindow::showMainWindow);
  QObject::connect(&guard, &SingleInstanceGuard::openFilesRequested, &window,
                   &MainWindow::openFiles);
  QObject::connect(&app, &Application::openFileRequested, &window,
                   [&window](const QString &p_filePath) {
                     window.openFiles(QStringList() << p_filePath);
                   });

  MarklyApp::getInst().initLoad();

  window.kickOffOnStart(cmdOptions.m_pathsToOpen);

  return app.exec();
}
