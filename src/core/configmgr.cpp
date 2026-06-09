#include "configmgr.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QTemporaryDir>

#include "coreconfig.h"
#include "mainconfig.h"
#include "sessionconfig.h"
#include <utils/fileutils.h>
#include <utils/pathutils.h>

using namespace markly;

const QString ConfigMgr::c_orgName = "Markly";

const QString ConfigMgr::c_appName = "Markly";

const QString ConfigMgr::c_configFileName = "markly.json";

const QString ConfigMgr::c_sessionFileName = "session.json";

const QString ConfigMgr::c_userFilesFolder = "user_files";

const QString ConfigMgr::c_appFilesFolder = "markly_files";

const QJsonObject &ConfigMgr::Settings::getJson() const { return m_jobj; }

QSharedPointer<ConfigMgr::Settings> ConfigMgr::Settings::fromFile(const QString &p_jsonFilePath) {
  if (!QFileInfo::exists(p_jsonFilePath)) {
    qDebug() << "return empty Settings from non-exist config file" << p_jsonFilePath;
    return QSharedPointer<Settings>::create();
  }

  auto bytes = FileUtils::readFile(p_jsonFilePath);
  QJsonParseError err;
  auto doc = QJsonDocument::fromJson(bytes, &err);
  if (err.error != QJsonParseError::NoError) {
    qWarning() << "corrupted config file" << p_jsonFilePath << err.errorString()
               << "- falling back to empty settings";
    return QSharedPointer<Settings>::create();
  }
  return QSharedPointer<Settings>::create(doc.object());
}

void ConfigMgr::Settings::writeToFile(const QString &p_jsonFilePath) const {
  FileUtils::writeFile(p_jsonFilePath, QJsonDocument(this->m_jobj).toJson());
}

ConfigMgr::ConfigMgr(bool p_isUnitTest, QObject *p_parent)
    : QObject(p_parent), m_config(new MainConfig(this)), m_sessionConfig(new SessionConfig(this)) {
  if (p_isUnitTest) {
    m_dirForUnitTest.reset(new QTemporaryDir());
    if (!m_dirForUnitTest->isValid()) {
      qWarning() << "failed to init ConfigMgr for UnitTest";
      return;
    }

    QDir dir(m_dirForUnitTest->path());
    dir.mkdir(c_appFilesFolder);
    dir.mkdir(c_userFilesFolder);

    m_appConfigFolderPath = m_dirForUnitTest->filePath(c_appFilesFolder);
    m_userConfigFolderPath = m_dirForUnitTest->filePath(c_userFilesFolder);

    m_config->init();
    m_sessionConfig->init();
    return;
  }

  locateConfigFolder();

  checkUserConfig();

  m_config->init();
  m_sessionConfig->init();
}

ConfigMgr::~ConfigMgr() {}

ConfigMgr &ConfigMgr::getInst(bool p_isUnitTest) {
  static ConfigMgr inst(p_isUnitTest);
  return inst;
}

void ConfigMgr::initForUnitTest() { getInst(true); }

void ConfigMgr::locateConfigFolder() {
  const auto appDirPath = getApplicationDirPath();
  qInfo() << "app folder" << appDirPath;

  // App config: portable folder next to the binary, else standard app data.
  {
    QString folderPath(appDirPath + '/' + c_appFilesFolder);
    if (QDir(folderPath).exists()) {
      m_appConfigFolderPath = PathUtils::cleanPath(folderPath);
    } else {
      m_appConfigFolderPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
  }

  // User config: portable folder next to the binary, else standard app config.
  {
    QString folderPath(appDirPath + '/' + c_userFilesFolder);
    if (QDir(folderPath).exists()) {
      m_userConfigFolderPath = PathUtils::cleanPath(folderPath);
    } else {
      m_userConfigFolderPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
      QDir().mkpath(m_userConfigFolderPath);
    }
  }

  qInfo() << "app config folder" << m_appConfigFolderPath;
  qInfo() << "user config folder" << m_userConfigFolderPath;
}

void ConfigMgr::checkUserConfig() {
  // Make sure user/session config files are writable if they exist.
  for (const auto &file : {getConfigFilePath(Source::User), getConfigFilePath(Source::Session)}) {
    if (QFileInfo::exists(file) && !(QFile::permissions(file) & QFile::WriteUser)) {
      qDebug() << "make config file writable" << file;
      QFile::setPermissions(file, QFile::WriteUser);
    }
  }
}

QString ConfigMgr::getConfigFilePath(Source p_src) const {
  switch (p_src) {
  case Source::Default:
    return getDefaultConfigFilePath();
  case Source::App:
    return m_appConfigFolderPath + QLatin1Char('/') + c_configFileName;
  case Source::User:
    return m_userConfigFolderPath + QLatin1Char('/') + c_configFileName;
  case Source::Session:
    return m_userConfigFolderPath + QLatin1Char('/') + c_sessionFileName;
  }
  Q_ASSERT(false);
  return QString();
}

QString ConfigMgr::getDefaultConfigFilePath() {
  return QStringLiteral(":/data/core/") + c_configFileName;
}

QSharedPointer<ConfigMgr::Settings> ConfigMgr::getSettings(Source p_src) const {
  return ConfigMgr::Settings::fromFile(getConfigFilePath(p_src));
}

void ConfigMgr::writeUserSettings(const QJsonObject &p_jobj) {
  Settings settings(p_jobj);
  settings.writeToFile(getConfigFilePath(Source::User));
}

void ConfigMgr::writeSessionSettings(const QJsonObject &p_jobj) {
  Settings settings(p_jobj);
  settings.writeToFile(getConfigFilePath(Source::Session));
}

MainConfig &ConfigMgr::getConfig() { return *m_config; }

SessionConfig &ConfigMgr::getSessionConfig() { return *m_sessionConfig; }

CoreConfig &ConfigMgr::getCoreConfig() { return m_config->getCoreConfig(); }

EditorConfig &ConfigMgr::getEditorConfig() { return m_config->getEditorConfig(); }

WidgetConfig &ConfigMgr::getWidgetConfig() { return m_config->getWidgetConfig(); }

QString ConfigMgr::getAppFolder() const { return m_appConfigFolderPath; }

QString ConfigMgr::getUserFolder() const { return m_userConfigFolderPath; }

static QString appSub(const QString &p_base, const QString &p_sub) {
  return PathUtils::concatenateFilePath(p_base, p_sub);
}

static QString userSub(const QString &p_base, const QString &p_sub) {
  auto folderPath = PathUtils::concatenateFilePath(p_base, p_sub);
  QDir().mkpath(folderPath);
  return folderPath;
}

QString ConfigMgr::getAppThemeFolder() const { return appSub(m_appConfigFolderPath, "themes"); }

QString ConfigMgr::getUserThemeFolder() const { return userSub(m_userConfigFolderPath, "themes"); }

QString ConfigMgr::getAppTaskFolder() const { return appSub(m_appConfigFolderPath, "tasks"); }

QString ConfigMgr::getUserTaskFolder() const { return userSub(m_userConfigFolderPath, "tasks"); }

QString ConfigMgr::getAppWebStylesFolder() const {
  return appSub(m_appConfigFolderPath, "web-styles");
}

QString ConfigMgr::getUserWebStylesFolder() const {
  return userSub(m_userConfigFolderPath, "web-styles");
}

QString ConfigMgr::getAppDocsFolder() const { return appSub(m_appConfigFolderPath, "docs"); }

QString ConfigMgr::getUserDocsFolder() const { return userSub(m_userConfigFolderPath, "docs"); }

QString ConfigMgr::getAppSyntaxHighlightingFolder() const {
  return appSub(m_appConfigFolderPath, "syntax-highlighting");
}

QString ConfigMgr::getUserSyntaxHighlightingFolder() const {
  return userSub(m_userConfigFolderPath, "syntax-highlighting");
}

QString ConfigMgr::getAppDictsFolder() const { return appSub(m_appConfigFolderPath, "dicts"); }

QString ConfigMgr::getUserDictsFolder() const { return userSub(m_userConfigFolderPath, "dicts"); }

QString ConfigMgr::getUserTemplateFolder() const {
  return userSub(m_userConfigFolderPath, "templates");
}

QString ConfigMgr::getUserSnippetFolder() const {
  return userSub(m_userConfigFolderPath, "snippets");
}

QString ConfigMgr::getUserMarkdownUserStyleFile() const {
  auto folderPath = PathUtils::concatenateFilePath(m_userConfigFolderPath, "web/css");
  auto filePath = PathUtils::concatenateFilePath(folderPath, "user.css");
  if (!QFileInfo::exists(filePath)) {
    QDir().mkpath(folderPath);
    FileUtils::writeFile(filePath, QByteArray());
  }
  return filePath;
}

QString ConfigMgr::getUserOrAppFile(const QString &p_filePath) const {
  QFileInfo fi(p_filePath);
  if (fi.isAbsolute()) {
    return p_filePath;
  }

  QDir userConfigDir(m_userConfigFolderPath);
  if (userConfigDir.exists(p_filePath)) {
    return userConfigDir.absoluteFilePath(p_filePath);
  }

  QDir appConfigDir(m_appConfigFolderPath);
  return appConfigDir.absoluteFilePath(p_filePath);
}

QString ConfigMgr::locateSessionConfigFilePathAtBootstrap() {
  // QApplication is not init yet, so org and app name are empty here.
  auto folderPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  folderPath = PathUtils::concatenateFilePath(folderPath, c_orgName + "/" + c_appName);
  QDir dir(folderPath);
  if (dir.exists(c_sessionFileName)) {
    qInfo() << "locateSessionConfigFilePathAtBootstrap" << folderPath;
    return dir.filePath(c_sessionFileName);
  }

  return QString();
}

QString ConfigMgr::getLogFile() const {
  return PathUtils::concatenateFilePath(getUserFolder(), "markly.log");
}

QString ConfigMgr::getApplicationFilePath() {
#if defined(Q_OS_LINUX)
  auto appImageVar = QString::fromLocal8Bit(qgetenv("APPIMAGE"));
  if (!appImageVar.isEmpty()) {
    return appImageVar;
  }
#elif defined(Q_OS_MACOS)
  auto exePath = QCoreApplication::applicationFilePath();
  const QString exeName = c_appName.toLower() + ".app";
  int idx = exePath.indexOf(exeName + QStringLiteral("/Contents/MacOS/"));
  if (idx != -1) {
    return exePath.left(idx + exeName.size());
  }
#endif

  return QCoreApplication::applicationFilePath();
}

QString ConfigMgr::getApplicationDirPath() {
  return PathUtils::parentDirPath(getApplicationFilePath());
}

QString ConfigMgr::getDocumentOrHomePath() {
  static QString docHomePath;
  if (docHomePath.isEmpty()) {
    QStringList folders = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    docHomePath = folders.isEmpty() ? QDir::homePath() : folders[0];
  }

  return docHomePath;
}

QString ConfigMgr::getApplicationVersion() {
  static QString appVersion;
  if (appVersion.isEmpty()) {
    auto defaultSettings = ConfigMgr::Settings::fromFile(getDefaultConfigFilePath());
    const auto &defaultObj = defaultSettings->getJson();
    auto metaDataObj = defaultObj.value(QStringLiteral("metadata")).toObject();
    appVersion = metaDataObj.value(QStringLiteral("version")).toString();
  }

  return appVersion;
}

void ConfigMgr::initAppPrefixPath() {
  // Support QFile("app:abc.txt").
  QStringList potentialDirs;
  auto appDirPath = QCoreApplication::applicationDirPath();
  qInfo() << "app prefix path:" << appDirPath;
  potentialDirs << appDirPath;

#if defined(Q_OS_LINUX)
  QDir localBinDir(appDirPath);
  if (localBinDir.exists("../share")) {
    potentialDirs << QDir::cleanPath(localBinDir.filePath("../share"));
  }
#elif defined(Q_OS_MACOS)
  QDir localBinDir(appDirPath);
  if (localBinDir.exists("../Resources")) {
    potentialDirs << QDir::cleanPath(localBinDir.filePath("../Resources"));
  }
#endif

  QDir::setSearchPaths("app", potentialDirs);
}

static QJsonValue readJsonByPath(const QJsonObject &p_root, const QString &p_path) {
  const auto keys = p_path.split(QLatin1Char('.'), Qt::SkipEmptyParts);
  QJsonValue cur(p_root);
  for (const auto &key : keys) {
    if (!cur.isObject()) {
      return QJsonValue();
    }
    cur = cur.toObject().value(key);
  }
  return cur;
}

QJsonValue ConfigMgr::parseAndReadConfig(const QString &p_exp) const {
  if (p_exp.startsWith(QStringLiteral("main."))) {
    return readJsonByPath(m_config->toJson(), p_exp.mid(5));
  } else if (p_exp.startsWith(QStringLiteral("session."))) {
    return readJsonByPath(m_sessionConfig->toJson(), p_exp.mid(8));
  }
  return QJsonValue();
}
