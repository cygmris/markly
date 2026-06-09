#ifndef MARKLY_GITEEIMAGEHOST_H
#define MARKLY_GITEEIMAGEHOST_H

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

namespace markly {
// Gitee image host via the Contents API (#10c). Differs from GitHub: POST, the token
// goes in the JSON body (access_token), and the base path is /api/v5. The response
// shape matches GitHub's, so GithubImageHost::parseDownloadUrl is reused.
class GiteeImageHost : public QObject {
  Q_OBJECT
public:
  explicit GiteeImageHost(QObject *p_parent = nullptr);

  // https://gitee.com/api/v5/repos/<user>/<repo>/contents/<path>
  static QString uploadUrl(const QString &p_user, const QString &p_repo, const QString &p_path);
  // JSON body: { access_token, message, content(base64), branch? }
  static QByteArray uploadBody(const QString &p_path, const QByteArray &p_data,
                               const QString &p_branch, const QString &p_token);

  // Async POST. Emits done(true, downloadUrl) or done(false, error).
  void upload(const QString &p_user, const QString &p_repo, const QString &p_token,
              const QString &p_branch, const QString &p_remotePath, const QByteArray &p_data);

signals:
  void done(bool p_ok, const QString &p_urlOrError);

private:
  QNetworkAccessManager m_nam;
};
} // namespace markly

#endif // MARKLY_GITEEIMAGEHOST_H
