#ifndef MARKLY_GITHUBIMAGEHOST_H
#define MARKLY_GITHUBIMAGEHOST_H

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

namespace markly {
// GitHub image host via the Contents API (#10b). The request/response builders are
// static pure functions (unit-testable); `upload` performs the async PUT.
class GithubImageHost : public QObject {
  Q_OBJECT
public:
  explicit GithubImageHost(QObject *p_parent = nullptr);

  // https://api.github.com/repos/<user>/<repo>/contents/<path>
  static QString uploadUrl(const QString &p_user, const QString &p_repo, const QString &p_path);
  // JSON body: { message, content(base64), branch? }
  static QByteArray uploadBody(const QString &p_path, const QByteArray &p_data,
                               const QString &p_branch);
  // "token <token>"
  static QString authHeader(const QString &p_token);
  // content.download_url, or "" if absent / unparsable.
  static QString parseDownloadUrl(const QByteArray &p_responseJson);

  // Async PUT. Emits done(true, downloadUrl) or done(false, error).
  void upload(const QString &p_user, const QString &p_repo, const QString &p_token,
              const QString &p_branch, const QString &p_remotePath, const QByteArray &p_data);

signals:
  void done(bool p_ok, const QString &p_urlOrError);

private:
  QNetworkAccessManager m_nam;
};
} // namespace markly

#endif // MARKLY_GITHUBIMAGEHOST_H
