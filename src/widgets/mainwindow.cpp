#include "mainwindow.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QIcon>
#include <QImage>
#include <QMenu>
#include <QQmlContext>
#include <QQmlEngine>
#include <QSystemTrayIcon>
#include <QTimer>

#include "editors/editinputqml.h"
#include "editors/editorcfgqml.h"
#include "editors/markdownhighlighter.h"
#include <QQuickWidget>
#include <QUrl>

#include "explorer/dialoghelper.h"
#include "explorer/notebookexplorer.h"
#include "export/exporthelper.h"
#include "export/pandocbridge.h"
#include "export/mergebridge.h"
#include "images/imagehelper.h"
#include "images/imagehostbridge.h"
#include "locale/localebridge.h"
#include "editors/vibridge.h"
#include "hotkey/globalhotkey.h"
#include "spell/spellbridge.h"
#include "update/updatebridge.h"
#include "update/traycfg.h"
#include "quick/quickbridge.h"
#include "search/searchbridge.h"
#include "snippet/snippetbridge.h"
#include "task/taskbridge.h"
#include "tags/tagbridge.h"
#include "viewarea/viewarea.h"
#include <core/configmgr.h>
#include <core/widgetconfig.h>
#include <core/marklyapp.h>
#include <core/sessionconfig.h>
#include <core/theme/appearance.h>
#include <core/thememgr.h>

using namespace markly;

MainWindow::MainWindow(QWidget *p_parent) : FramelessMainWindow(p_parent) {
  setWindowTitle(QStringLiteral("Markly"));

  setupContent();
  setupTray();

  // Global hotkey to summon the window (#17b; X11 only, no-op on Wayland).
  m_hotkey = new GlobalHotkey(this);
  m_hotkey->registerHotkey(ConfigMgr::getInst().getWidgetConfig().getGlobalHotkey());
  connect(m_hotkey, &GlobalHotkey::activated, this, &MainWindow::showMainWindow);

  resize(1100, 720);
  loadStateAndGeometry();
}

void MainWindow::setupTray() {
  // No system tray (headless / offscreen / unsupported DE): skip gracefully (#20b).
  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    return;
  }
  m_trayIcon = new QSystemTrayIcon(QIcon(QStringLiteral(":/markly.png")), this);
  m_trayIcon->setToolTip(QStringLiteral("Markly"));
  auto *menu = new QMenu(this);
  menu->addAction(tr("显示 Markly"), this, &MainWindow::showMainWindow);
  menu->addAction(tr("退出"), qApp, &QCoreApplication::quit);
  m_trayIcon->setContextMenu(menu);
  connect(m_trayIcon, &QSystemTrayIcon::activated, this,
          [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
              showMainWindow();
            }
          });
  m_trayIcon->show();
}

void MainWindow::setupContent() {
  auto &themeMgr = MarklyApp::getInst().getThemeMgr();

  m_quick = new QQuickWidget(this);
  m_quick->setResizeMode(QQuickWidget::SizeRootObjectToView);
  m_quick->rootContext()->setContextProperty(QStringLiteral("Theme"), &themeMgr);
  m_quick->rootContext()->setContextProperty(QStringLiteral("Appearance"),
                                             themeMgr.getAppearance());
  // Window controls for the self-drawn title bar.
  m_quick->rootContext()->setContextProperty(QStringLiteral("Win"), this);
  // Notebook explorer bridge + native dialogs.
  auto *explorer = new NotebookExplorer(MarklyApp::getInst().getNotebookMgr(), this);
  auto *dialogs = new DialogHelper(this, this);
  m_quick->rootContext()->setContextProperty(QStringLiteral("Explorer"), explorer);
  m_quick->rootContext()->setContextProperty(QStringLiteral("Dialogs"), dialogs);
  // View/tab/split area; catches openFileRequested.
  m_views = new ViewArea(MarklyApp::getInst().getBufferMgr(), this);
  m_quick->rootContext()->setContextProperty(QStringLiteral("Views"), m_views);
  connect(&MarklyApp::getInst(), &MarklyApp::openFileRequested, m_views, &ViewArea::openFile);
  // Full-text search bridge (#12).
  auto *search = new SearchBridge(MarklyApp::getInst().getNotebookMgr(), this);
  m_quick->rootContext()->setContextProperty(QStringLiteral("Search"), search);
  // Tags panel bridge (#11).
  auto *tags = new TagBridge(MarklyApp::getInst().getNotebookMgr(), this);
  m_quick->rootContext()->setContextProperty(QStringLiteral("Tags"), tags);
  // Local image paste/drop bridge (#10).
  m_quick->rootContext()->setContextProperty(QStringLiteral("Images"),
                                             new ImageHelper(this));
  // Image host upload bridges (#10b).
  m_quick->rootContext()->setContextProperty(QStringLiteral("ImageHost"),
                                             new ImageHostBridge(this));
  m_quick->rootContext()->setContextProperty(QStringLiteral("ImageHostCfg"),
                                             new ImageHostCfgQml(this));
  // Quick access / history / flash bridge (#13).
  m_quick->rootContext()->setContextProperty(
      QStringLiteral("Quick"), new QuickBridge(MarklyApp::getInst().getHistoryMgr(), this));
  // Snippets bridge (#14).
  m_quick->rootContext()->setContextProperty(
      QStringLiteral("Snippets"), new SnippetBridge(MarklyApp::getInst().getSnippetMgr(), this));
  // Export bridge (#15).
  m_quick->rootContext()->setContextProperty(QStringLiteral("Export"), new ExportHelper(this));
  // Pandoc custom-format export bridge (#15b).
  m_quick->rootContext()->setContextProperty(QStringLiteral("Pandoc"), new PandocBridge(this));
  // Merged folder export bridge (#15c).
  m_quick->rootContext()->setContextProperty(QStringLiteral("Merge"), new MergeBridge(this));
  // Task bridge (#16).
  m_quick->rootContext()->setContextProperty(
      QStringLiteral("Tasks"), new TaskBridge(MarklyApp::getInst().getTaskMgr(),
                                              MarklyApp::getInst().getNotebookMgr(), this));
  // Markdown editor: QML-instantiable highlighter + editor config bridge.
  qmlRegisterType<MarkdownHighlighter>("Markly.Editor", 1, 0, "MarkdownHighlighter");
  m_quick->rootContext()->setContextProperty(QStringLiteral("EditorCfg"),
                                             new EditorCfgQml(this));
  m_quick->rootContext()->setContextProperty(QStringLiteral("EditInput"),
                                             new EditInputQml(this));
  // Spell-check right-click bridge (#8d).
  m_quick->rootContext()->setContextProperty(QStringLiteral("Spell"),
                                             new SpellBridge(this));
  // Vi input bridge (#8b).
  m_quick->rootContext()->setContextProperty(QStringLiteral("Vi"),
                                             new ViBridge(this));
  // Update check + tray config bridges (#20b).
  m_quick->rootContext()->setContextProperty(QStringLiteral("Update"),
                                             new UpdateBridge(this));
  m_quick->rootContext()->setContextProperty(QStringLiteral("TrayCfg"),
                                             new TrayCfgQml(this));
  // Language switch bridge (#20).
  m_quick->rootContext()->setContextProperty(QStringLiteral("Locale"),
                                             new LocaleBridge(m_quick->engine(), this));
  m_quick->setSource(QUrl(QStringLiteral("qrc:/qml/MarklyShell.qml")));
  if (m_quick->status() == QQuickWidget::Error) {
    qCritical() << "failed to load MarklyShell.qml:" << m_quick->errors();
  }
  setCentralWidget(m_quick);

  // Keep the QWidget chrome in sync with the active theme.
  connect(&themeMgr, &ThemeMgr::themeChanged, this, []() {
    qApp->setStyleSheet(MarklyApp::getInst().getThemeMgr().fetchQtStyleSheet());
  });

  // Dev screenshot hook: MARKLY_SHOT=/path.png grabs the shell then quits.
  const auto shotPath = qEnvironmentVariable("MARKLY_SHOT");
  if (!shotPath.isEmpty()) {
    const auto shotSearch = qEnvironmentVariable("MARKLY_SHOT_SEARCH");
    if (!shotSearch.isEmpty()) {
      QTimer::singleShot(800, this, [this, shotSearch]() {
        if (auto *root = m_quick->rootObject()) {
          QMetaObject::invokeMethod(root, "showSearch", Q_ARG(QVariant, shotSearch));
        }
      });
    }
    const auto shotTags = qEnvironmentVariable("MARKLY_SHOT_TAGS");
    if (!shotTags.isEmpty()) {
      QTimer::singleShot(800, this, [this, shotTags]() {
        if (auto *root = m_quick->rootObject()) {
          QMetaObject::invokeMethod(root, "showTags", Q_ARG(QVariant, shotTags));
        }
      });
    }
    if (!qEnvironmentVariable("MARKLY_SHOT_QUICK").isEmpty()) {
      QTimer::singleShot(800, this, [this]() {
        if (auto *root = m_quick->rootObject()) {
          QMetaObject::invokeMethod(root, "showQuick");
        }
      });
    }
    if (!qEnvironmentVariable("MARKLY_SHOT_SNIPPET").isEmpty()) {
      QTimer::singleShot(800, this, [this]() {
        if (auto *root = m_quick->rootObject()) {
          QMetaObject::invokeMethod(root, "showSnippet");
        }
      });
    }
    if (!qEnvironmentVariable("MARKLY_SHOT_SETTINGS").isEmpty()) {
      QTimer::singleShot(800, this, [this]() {
        if (auto *root = m_quick->rootObject()) {
          QMetaObject::invokeMethod(root, "showSettings");
        }
      });
    }
    if (!qEnvironmentVariable("MARKLY_SHOT_ENTRY").isEmpty()) {
      QTimer::singleShot(800, this, [this]() {
        if (auto *root = m_quick->rootObject()) {
          QMetaObject::invokeMethod(root, "showEntry");
        }
      });
    }
    QTimer::singleShot(1200, this, [this, shotPath]() {
      const QImage img = m_quick->grabFramebuffer();
      if (img.save(shotPath)) {
        qInfo() << "saved screenshot" << shotPath << img.size();
      } else {
        qWarning() << "failed to save screenshot" << shotPath;
      }
      qApp->quit();
    });
  }

  // Offline export validation: MARKLY_EXPORT="fmt:path" exports the active note then quits.
  const auto exportSpec = qEnvironmentVariable("MARKLY_EXPORT");
  if (!exportSpec.isEmpty()) {
    const int sep = exportSpec.indexOf(QLatin1Char(':'));
    if (sep > 0) {
      const auto fmt = exportSpec.left(sep);
      const auto path = exportSpec.mid(sep + 1);
      QTimer::singleShot(1000, this, [this, fmt, path]() {
        if (auto *root = m_quick->rootObject()) {
          QMetaObject::invokeMethod(root, "exportNoteTo", Q_ARG(QVariant, fmt),
                                    Q_ARG(QVariant, path));
        }
      });
      QTimer::singleShot(2500, qApp, &QCoreApplication::quit);
    }
  }
}

void MainWindow::loadStateAndGeometry() {
  auto &session = ConfigMgr::getInst().getSessionConfig();
  const auto geometry = session.getMainWindowGeometry();
  if (!geometry.isEmpty()) {
    restoreGeometry(geometry);
  }
  const auto state = session.getMainWindowState();
  if (!state.isEmpty()) {
    restoreState(state);
  }
}

void MainWindow::saveStateAndGeometry() {
  auto &session = ConfigMgr::getInst().getSessionConfig();
  session.setMainWindowGeometry(saveGeometry());
  session.setMainWindowState(saveState());
}

void MainWindow::kickOffOnStart(const QStringList &p_paths) {
  // Restore previously open tabs.
  if (m_views) {
    m_views->restoreSession();
  }
  if (!p_paths.isEmpty()) {
    openFiles(p_paths);
  }
}

void MainWindow::showMainWindow() {
  setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
  show();
  raise();
  activateWindow();
}

void MainWindow::openFiles(const QStringList &p_files) {
  // Foundation phase: real opening lands in spec #6 (buffer-view-area).
  for (const auto &file : p_files) {
    qInfo() << "request to open file" << file;
  }
}

void MainWindow::closeEvent(QCloseEvent *p_event) {
  // Minimize to tray instead of quitting, when enabled and a tray is available (#20b).
  if (m_trayIcon && m_trayIcon->isVisible() &&
      ConfigMgr::getInst().getWidgetConfig().getMinimizeToTray()) {
    hide();
    p_event->ignore();
    return;
  }
  saveStateAndGeometry();
  QMainWindow::closeEvent(p_event);
}
