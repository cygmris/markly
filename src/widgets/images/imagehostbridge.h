#ifndef MARKLY_IMAGEHOSTBRIDGE_H
#define MARKLY_IMAGEHOSTBRIDGE_H

#include <QObject>
#include <QString>

#include <core/imagehost/githubimagehost.h>
#include <core/imagehost/giteeimagehost.h>
#include <core/imagehost/repoimagehost.h>

namespace markly {
// QML bridge (context property "ImageHost", #10b) to upload a note's local images to
// the configured GitHub repo and report the resulting public URL.
class ImageHostBridge : public QObject {
  Q_OBJECT
public:
  explicit ImageHostBridge(QObject *p_parent = nullptr);

  // type == "github" and user/repo/token all set.
  Q_INVOKABLE bool configured() const;

  // Resolve localPath against baseDir, read bytes, upload; emits uploaded/failed.
  Q_INVOKABLE void upload(const QString &p_localPath, const QString &p_baseDir);

signals:
  void uploaded(const QString &p_localPath, const QString &p_url);
  void failed(const QString &p_localPath, const QString &p_error);

private:
  GithubImageHost m_host;
  GiteeImageHost m_gitee;
};

// QML bridge (context property "ImageHostCfg") to read/write WidgetConfig.image_host.
class ImageHostCfgQml : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString type READ type WRITE setType NOTIFY changed)
  Q_PROPERTY(QString user READ user WRITE setUser NOTIFY changed)
  Q_PROPERTY(QString repo READ repo WRITE setRepo NOTIFY changed)
  Q_PROPERTY(QString token READ token WRITE setToken NOTIFY changed)
  Q_PROPERTY(QString branch READ branch WRITE setBranch NOTIFY changed)
  Q_PROPERTY(QString localRepo READ localRepo WRITE setLocalRepo NOTIFY changed)
  Q_PROPERTY(QString rawBase READ rawBase WRITE setRawBase NOTIFY changed)
  Q_PROPERTY(QString subDir READ subDir WRITE setSubDir NOTIFY changed)
  Q_PROPERTY(QString cloneUrl READ cloneUrl WRITE setCloneUrl NOTIFY changed)
public:
  explicit ImageHostCfgQml(QObject *p_parent = nullptr);

  QString type() const;
  QString user() const;
  QString repo() const;
  QString token() const;
  QString branch() const;
  QString localRepo() const;
  QString rawBase() const;
  QString subDir() const;
  QString cloneUrl() const;

  void setType(const QString &p_v);
  void setUser(const QString &p_v);
  void setRepo(const QString &p_v);
  void setToken(const QString &p_v);
  void setBranch(const QString &p_v);
  void setLocalRepo(const QString &p_v);
  void setRawBase(const QString &p_v);
  void setSubDir(const QString &p_v);
  void setCloneUrl(const QString &p_v);

signals:
  void changed();

private:
  QString field(const QString &p_key) const;
  void setField(const QString &p_key, const QString &p_v);
};
} // namespace markly

#endif // MARKLY_IMAGEHOSTBRIDGE_H
