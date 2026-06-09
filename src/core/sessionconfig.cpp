#include "sessionconfig.h"

#include <QJsonObject>

#include "configmgr.h"

using namespace markly;

SessionConfig::SessionConfig(ConfigMgr *p_mgr) : IConfig(p_mgr, nullptr) {}

SessionConfig::~SessionConfig() {}

void SessionConfig::init() {
  m_sessionName = QStringLiteral("session");

  auto settings = getMgr()->getSettings(ConfigMgr::Source::Session);
  const QJsonObject &obj = settings->getJson();

  m_mainWindowGeometry = readByteArray(obj, QStringLiteral("main_window_geometry"));
  m_mainWindowState = readByteArray(obj, QStringLiteral("main_window_state"));
  m_newNotebookDefaultRootFolderPath =
      readString(obj, QStringLiteral("new_notebook_default_root_folder_path"));
  m_notebookRootPaths = readStringList(obj, QStringLiteral("notebook_root_paths"));
  m_currentNotebookRootPath = readString(obj, QStringLiteral("current_notebook_root_path"));
}

QJsonObject SessionConfig::toJson() const {
  QJsonObject obj;
  writeByteArray(obj, QStringLiteral("main_window_geometry"), m_mainWindowGeometry);
  writeByteArray(obj, QStringLiteral("main_window_state"), m_mainWindowState);
  obj[QStringLiteral("new_notebook_default_root_folder_path")] =
      m_newNotebookDefaultRootFolderPath;
  writeStringList(obj, QStringLiteral("notebook_root_paths"), m_notebookRootPaths);
  obj[QStringLiteral("current_notebook_root_path")] = m_currentNotebookRootPath;
  return obj;
}

void SessionConfig::writeToSettings() const { getMgr()->writeSessionSettings(toJson()); }

QByteArray SessionConfig::getMainWindowGeometry() const { return m_mainWindowGeometry; }

void SessionConfig::setMainWindowGeometry(const QByteArray &p_geometry) {
  updateConfig(m_mainWindowGeometry, p_geometry, this);
}

QByteArray SessionConfig::getMainWindowState() const { return m_mainWindowState; }

void SessionConfig::setMainWindowState(const QByteArray &p_state) {
  updateConfig(m_mainWindowState, p_state, this);
}

const QString &SessionConfig::getNewNotebookDefaultRootFolderPath() const {
  return m_newNotebookDefaultRootFolderPath;
}

void SessionConfig::setNewNotebookDefaultRootFolderPath(const QString &p_path) {
  updateConfig(m_newNotebookDefaultRootFolderPath, p_path, this);
}

QStringList SessionConfig::getNotebookRootPaths() const { return m_notebookRootPaths; }

void SessionConfig::setNotebookRootPaths(const QStringList &p_paths) {
  updateConfig(m_notebookRootPaths, p_paths, this);
}

QString SessionConfig::getCurrentNotebookRootPath() const { return m_currentNotebookRootPath; }

void SessionConfig::setCurrentNotebookRootPath(const QString &p_path) {
  updateConfig(m_currentNotebookRootPath, p_path, this);
}
