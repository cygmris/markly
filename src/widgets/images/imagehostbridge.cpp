#include "imagehostbridge.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QUrl>

#include <core/configmgr.h>
#include <core/widgetconfig.h>

using namespace markly;

namespace {
QJsonObject hostCfg() { return ConfigMgr::getInst().getWidgetConfig().getImageHostConfig(); }
} // namespace

ImageHostBridge::ImageHostBridge(QObject *p_parent) : QObject(p_parent) {}

bool ImageHostBridge::configured() const {
  const QJsonObject c = hostCfg();
  const QString type = c.value(QStringLiteral("type")).toString();
  if (type == QStringLiteral("repo")) {
    return !c.value(QStringLiteral("local_repo")).toString().isEmpty() &&
           !c.value(QStringLiteral("raw_base")).toString().isEmpty();
  }
  if (type != QStringLiteral("github") && type != QStringLiteral("gitee")) {
    return false;
  }
  return !c.value(QStringLiteral("user")).toString().isEmpty() &&
         !c.value(QStringLiteral("repo")).toString().isEmpty() &&
         !c.value(QStringLiteral("token")).toString().isEmpty();
}

void ImageHostBridge::upload(const QString &p_localPath, const QString &p_baseDir) {
  if (!configured()) {
    emit failed(p_localPath, QStringLiteral("image host not configured"));
    return;
  }
  // Resolve to an absolute path (relative paths are against the note's directory).
  QString abs = p_localPath;
  if (abs.startsWith(QStringLiteral("file://"))) {
    abs = QUrl(abs).toLocalFile();
  }
  if (QFileInfo(abs).isRelative() && !p_baseDir.isEmpty()) {
    abs = QDir(p_baseDir).absoluteFilePath(abs);
  }
  QFile f(abs);
  if (!f.open(QIODevice::ReadOnly)) {
    emit failed(p_localPath, QStringLiteral("cannot read %1").arg(abs));
    return;
  }
  const QByteArray data = f.readAll();
  f.close();

  const QJsonObject c = hostCfg();
  const QString remotePath = QFileInfo(abs).fileName();
  const QString type = c.value(QStringLiteral("type")).toString();
  const QString user = c.value(QStringLiteral("user")).toString();
  const QString repo = c.value(QStringLiteral("repo")).toString();
  const QString token = c.value(QStringLiteral("token")).toString();
  const QString branch = c.value(QStringLiteral("branch")).toString();

  auto onDone = [this, p_localPath](bool ok, const QString &urlOrError) {
    if (ok) {
      emit uploaded(p_localPath, urlOrError);
    } else {
      emit failed(p_localPath, urlOrError);
    }
  };

  if (type == QStringLiteral("repo")) {
    const QString localRepo = c.value(QStringLiteral("local_repo")).toString();
    QString cloneErr;
    if (!RepoImageHost::ensureCloned(c.value(QStringLiteral("clone_url")).toString(), localRepo,
                                     &cloneErr)) {
      emit failed(p_localPath, cloneErr);
      return;
    }
    const QString subDir = c.value(QStringLiteral("sub_dir")).toString();
    const QString rel = subDir.isEmpty() ? remotePath : (subDir + QStringLiteral("/") + remotePath);
    QString err;
    if (RepoImageHost::commitImage(c.value(QStringLiteral("local_repo")).toString(), rel, data,
                                   QStringLiteral("MARKLY_ADD: %1").arg(remotePath), &err)) {
      RepoImageHost::push(c.value(QStringLiteral("local_repo")).toString()); // best-effort
      emit uploaded(p_localPath, RepoImageHost::rawUrl(c.value(QStringLiteral("raw_base")).toString(),
                                                       branch, rel));
    } else {
      emit failed(p_localPath, err);
    }
    return;
  }
  if (type == QStringLiteral("gitee")) {
    auto *conn = new QMetaObject::Connection;
    *conn = connect(&m_gitee, &GiteeImageHost::done, this,
                    [conn, onDone](bool ok, const QString &u) { disconnect(*conn); delete conn; onDone(ok, u); });
    m_gitee.upload(user, repo, token, branch, remotePath, data);
  } else {
    auto *conn = new QMetaObject::Connection;
    *conn = connect(&m_host, &GithubImageHost::done, this,
                    [conn, onDone](bool ok, const QString &u) { disconnect(*conn); delete conn; onDone(ok, u); });
    m_host.upload(user, repo, token, branch, remotePath, data);
  }
}

// ---- ImageHostCfgQml ----

ImageHostCfgQml::ImageHostCfgQml(QObject *p_parent) : QObject(p_parent) {}

QString ImageHostCfgQml::field(const QString &p_key) const {
  return hostCfg().value(p_key).toString();
}

void ImageHostCfgQml::setField(const QString &p_key, const QString &p_v) {
  QJsonObject c = hostCfg();
  if (c.value(p_key).toString() == p_v) {
    return;
  }
  c[p_key] = p_v;
  ConfigMgr::getInst().getWidgetConfig().setImageHostConfig(c);
  emit changed();
}

QString ImageHostCfgQml::type() const { return field(QStringLiteral("type")); }
QString ImageHostCfgQml::user() const { return field(QStringLiteral("user")); }
QString ImageHostCfgQml::repo() const { return field(QStringLiteral("repo")); }
QString ImageHostCfgQml::token() const { return field(QStringLiteral("token")); }
QString ImageHostCfgQml::branch() const { return field(QStringLiteral("branch")); }
QString ImageHostCfgQml::localRepo() const { return field(QStringLiteral("local_repo")); }
QString ImageHostCfgQml::rawBase() const { return field(QStringLiteral("raw_base")); }
QString ImageHostCfgQml::subDir() const { return field(QStringLiteral("sub_dir")); }
QString ImageHostCfgQml::cloneUrl() const { return field(QStringLiteral("clone_url")); }

void ImageHostCfgQml::setType(const QString &p_v) { setField(QStringLiteral("type"), p_v); }
void ImageHostCfgQml::setUser(const QString &p_v) { setField(QStringLiteral("user"), p_v); }
void ImageHostCfgQml::setRepo(const QString &p_v) { setField(QStringLiteral("repo"), p_v); }
void ImageHostCfgQml::setToken(const QString &p_v) { setField(QStringLiteral("token"), p_v); }
void ImageHostCfgQml::setBranch(const QString &p_v) { setField(QStringLiteral("branch"), p_v); }
void ImageHostCfgQml::setLocalRepo(const QString &p_v) { setField(QStringLiteral("local_repo"), p_v); }
void ImageHostCfgQml::setRawBase(const QString &p_v) { setField(QStringLiteral("raw_base"), p_v); }
void ImageHostCfgQml::setSubDir(const QString &p_v) { setField(QStringLiteral("sub_dir"), p_v); }
void ImageHostCfgQml::setCloneUrl(const QString &p_v) { setField(QStringLiteral("clone_url"), p_v); }
