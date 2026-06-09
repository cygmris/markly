#include "giteeimagehost.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "githubimagehost.h" // reuse parseDownloadUrl

using namespace markly;

GiteeImageHost::GiteeImageHost(QObject *p_parent) : QObject(p_parent) {}

QString GiteeImageHost::uploadUrl(const QString &p_user, const QString &p_repo,
                                  const QString &p_path) {
  return QStringLiteral("https://gitee.com/api/v5/repos/%1/%2/contents/%3")
      .arg(p_user, p_repo, p_path);
}

QByteArray GiteeImageHost::uploadBody(const QString &p_path, const QByteArray &p_data,
                                      const QString &p_branch, const QString &p_token) {
  QJsonObject obj;
  obj[QStringLiteral("access_token")] = p_token;
  obj[QStringLiteral("message")] = QStringLiteral("MARKLY_ADD: %1").arg(p_path);
  obj[QStringLiteral("content")] = QString::fromUtf8(p_data.toBase64());
  if (!p_branch.isEmpty()) {
    obj[QStringLiteral("branch")] = p_branch;
  }
  return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

void GiteeImageHost::upload(const QString &p_user, const QString &p_repo, const QString &p_token,
                            const QString &p_branch, const QString &p_remotePath,
                            const QByteArray &p_data) {
  QNetworkRequest req((QUrl(uploadUrl(p_user, p_repo, p_remotePath))));
  req.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/json"));

  QNetworkReply *reply = m_nam.post(req, uploadBody(p_remotePath, p_data, p_branch, p_token));
  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    const QByteArray body = reply->readAll();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();
    if (reply->error() == QNetworkReply::NoError && (status == 200 || status == 201)) {
      const QString url = GithubImageHost::parseDownloadUrl(body);
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
