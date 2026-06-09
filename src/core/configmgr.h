#ifndef CONFIGMGR_H
#define CONFIGMGR_H

#include <QJsonObject>
#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>

#include "noncopyable.h"

class QTemporaryDir;

namespace markly {
class MainConfig;
class SessionConfig;
class CoreConfig;
class EditorConfig;
class WidgetConfig;

class ConfigMgr : public QObject, private Noncopyable {
  Q_OBJECT
public:
  enum class Source { Default, App, User, Session };

  class Settings {
  public:
    Settings() = default;

    Settings(const QJsonObject &p_jobj) : m_jobj(p_jobj) {}

    const QJsonObject &getJson() const;

    void writeToFile(const QString &p_jsonFilePath) const;

    static QSharedPointer<Settings> fromFile(const QString &p_jsonFilePath);

  private:
    QJsonObject m_jobj;
  };

  ~ConfigMgr();

  static ConfigMgr &getInst(bool p_isUnitTest = false);

  MainConfig &getConfig();

  SessionConfig &getSessionConfig();

  CoreConfig &getCoreConfig();

  EditorConfig &getEditorConfig();

  WidgetConfig &getWidgetConfig();

  QString getAppFolder() const;

  QString getUserFolder() const;

  QString getLogFile() const;

  QString getAppThemeFolder() const;

  QString getUserThemeFolder() const;

  QString getAppTaskFolder() const;

  QString getUserTaskFolder() const;

  QString getAppWebStylesFolder() const;

  QString getUserWebStylesFolder() const;

  QString getAppDocsFolder() const;

  QString getUserDocsFolder() const;

  QString getAppSyntaxHighlightingFolder() const;

  QString getUserSyntaxHighlightingFolder() const;

  QString getAppDictsFolder() const;
  QString getUserDictsFolder() const;

  QString getUserTemplateFolder() const;

  QString getUserSnippetFolder() const;

  // web/css/user.css.
  QString getUserMarkdownUserStyleFile() const;

  // If @p_filePath is absolute, just return it.
  // Otherwise, first try to find it in user folder, then in app folder.
  QString getUserOrAppFile(const QString &p_filePath) const;

  QString getConfigFilePath(Source p_src) const;

  // Parse exp like "main.core.locale" and return the config value.
  QJsonValue parseAndReadConfig(const QString &p_exp) const;

  // Called at bootstrap without QApplication instance.
  static QString locateSessionConfigFilePathAtBootstrap();

  static QString getApplicationFilePath();

  static QString getApplicationDirPath();

  static QString getDocumentOrHomePath();

  static QString getApplicationVersion();

  static void initAppPrefixPath();

  static void initForUnitTest();

  static const QString c_orgName;

  static const QString c_appName;

public:
  // Used by IConfig.
  QSharedPointer<Settings> getSettings(Source p_src) const;

  void writeUserSettings(const QJsonObject &p_jobj);

  void writeSessionSettings(const QJsonObject &p_jobj);

signals:
  void editorConfigChanged();

private:
  ConfigMgr(bool p_isUnitTest, QObject *p_parent = nullptr);

  // Locate the folder path where the config file exists.
  void locateConfigFolder();

  void checkUserConfig();

  static QString getDefaultConfigFilePath();

  QScopedPointer<MainConfig> m_config;

  // Session config.
  QScopedPointer<SessionConfig> m_sessionConfig;

  // Absolute path of the app config folder.
  QString m_appConfigFolderPath;

  // Absolute path of the user config folder.
  QString m_userConfigFolderPath;

  // In UnitTest, we use a temp dir to hold the user files and app files.
  QScopedPointer<QTemporaryDir> m_dirForUnitTest;

  static const QString c_configFileName;

  static const QString c_sessionFileName;

  static const QString c_userFilesFolder;

  static const QString c_appFilesFolder;
};
} // namespace markly

#endif // CONFIGMGR_H
