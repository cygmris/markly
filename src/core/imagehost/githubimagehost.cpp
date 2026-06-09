#include "githubimagehost.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>

using namespace markly;

GithubImageHost::GithubImageHost(QObject *p_parent) : QObject(p_parent) {}

QString GithubImageHost::uploadUrl(const QString &p_user, const QString &p_repo,
                                   const QString &p_path) {
  return QStringLiteral("https://api.github.com/repos/%1/%2/contents/%3")
      .arg(p_user, p_repo, p_path);
}

QByteArray GithubImageHost::uploadBody(const QString &p_path, const QByteArray &p_data,
                                       const QString &p_branch) {
  QJsonObject obj;
  obj[QStringLiteral("message")] = QStringLiteral("MARKLY_ADD: %1").arg(p_path);
  obj[QStringLiteral("content")] = QString::fromUtf8(p_data.toBase64());
  if (!p_branch.isEmpty()) {
    obj[QStringLiteral("branch")] = p_branch;
  }
  return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

QString GithubImageHost::authHeader(const QString &p_token) {
  return QStringLiteral("token %1").arg(p_token);
}

QString GithubImageHost::parseDownloadUrl(const QByteArray &p_responseJson) {
  const auto doc = QJsonDocument::fromJson(p_responseJson);
  if (!doc.isObject()) {
    return QString();
  }
  return doc.object()
      .value(QStringLiteral("content"))
      .toObject()
      .value(QStringLiteral("download_url"))
      .toString();
}

void GithubImageHost::upload(const QString &p_user, const QString &p_repo, const QString &p_token,
                             const QString &p_branch, const QString &p_remotePath,
                             const QByteArray &p_data) {
  QNetworkRequest req((QUrl(uploadUrl(p_user, p_repo, p_remotePath))));
  req.setRawHeader("Authorization", authHeader(p_token).toUtf8());
  req.setRawHeader("Accept", "application/vnd.github+json");
  req.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/json"));

  QNetworkReply *reply = m_nam.put(req, uploadBody(p_remotePath, p_data, p_branch));
  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    const QByteArray body = reply->readAll();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();
    if (reply->error() == QNetworkReply::NoError && (status == 200 || status == 201)) {
      const QString url = parseDownloadUrl(body);
      if (!url.isEmpty()) {
        emit done(true, url);
        return;
      }
      emit done(false, QStringLiteral("upload succeeded but no download_url in response"));
      return;
    }
    emit done(false, QStringLiteral("HTTP %1: %2").arg(status).arg(QString::fromUtf8(body)));
  });
}
