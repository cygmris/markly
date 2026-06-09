#include "mainconfig.h"

#include <QJsonObject>

#include "configmgr.h"
#include "coreconfig.h"
#include "editorconfig.h"
#include "widgetconfig.h"

using namespace markly;

bool MainConfig::s_versionChanged = false;

MainConfig::MainConfig(ConfigMgr *p_mgr)
    : IConfig(p_mgr, nullptr), m_coreConfig(new CoreConfig(p_mgr, this)),
      m_editorConfig(new EditorConfig(p_mgr, this)), m_widgetConfig(new WidgetConfig(p_mgr, this)) {
}

MainConfig::~MainConfig() {}

void MainConfig::init() {
  m_sessionName = QStringLiteral("main");

  auto mgr = getMgr();
  auto defaultSettings = mgr->getSettings(ConfigMgr::Source::Default);
  auto userSettings = mgr->getSettings(ConfigMgr::Source::User);

  const QJsonObject &appObj = defaultSettings->getJson();
  const QJsonObject &userObj = userSettings->getJson();

  loadMetadata(appObj, userObj);

  m_coreConfig->init(appObj.value(QStringLiteral("core")).toObject(),
                     userObj.value(QStringLiteral("core")).toObject());
  m_editorConfig->init(appObj.value(QStringLiteral("editor")).toObject(),
                       userObj.value(QStringLiteral("editor")).toObject());
  m_widgetConfig->init(appObj.value(QStringLiteral("widget")).toObject(),
                       userObj.value(QStringLiteral("widget")).toObject());

  // Persist back so that a missing user config file gets created on first run.
  writeToSettings();
}

void MainConfig::loadMetadata(const QJsonObject &p_app, const QJsonObject &p_user) {
  m_version = getVersion(p_app);
  m_userVersion = getVersion(p_user);
  if (m_userVersion.isEmpty()) {
    m_userVersion = m_version;
  }
  s_versionChanged = (m_version != m_userVersion);
  if (s_versionChanged) {
    qInfo() << "config version changed from" << m_userVersion << "to" << m_version;
  }
}

QString MainConfig::getVersion(const QJsonObject &p_jobj) {
  return p_jobj.value(QStringLiteral("metadata"))
      .toObject()
      .value(QStringLiteral("version"))
      .toString();
}

bool MainConfig::isVersionChanged() { return s_versionChanged; }

QJsonObject MainConfig::saveMetadata() const {
  QJsonObject obj;
  obj[QStringLiteral("version")] = m_version;
  return obj;
}

QJsonObject MainConfig::toJson() const {
  QJsonObject obj;
  obj[QStringLiteral("metadata")] = saveMetadata();
  obj[QStringLiteral("version")] = m_version;
  obj[QStringLiteral("core")] = m_coreConfig->toJson();
  obj[QStringLiteral("editor")] = m_editorConfig->toJson();
  obj[QStringLiteral("widget")] = m_widgetConfig->toJson();
  return obj;
}

void MainConfig::writeToSettings() const { getMgr()->writeUserSettings(toJson()); }

const QString &MainConfig::getVersion() const { return m_version; }

CoreConfig &MainConfig::getCoreConfig() { return *m_coreConfig; }

EditorConfig &MainConfig::getEditorConfig() { return *m_editorConfig; }

WidgetConfig &MainConfig::getWidgetConfig() { return *m_widgetConfig; }
