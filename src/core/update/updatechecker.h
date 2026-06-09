#ifndef MARKLY_UPDATECHECKER_H
#define MARKLY_UPDATECHECKER_H

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

namespace markly {
// Checks GitHub Releases for a newer version (#20b). Version parsing/compare are static
// pure functions (unit-testable); check() does the async GET.
class UpdateChecker : public QObject {
  Q_OBJECT
public:
  explicit UpdateChecker(QObject *p_parent = nullptr);

  // Extract tag_name from the releases/latest JSON, stripping a leading 'v'. "" if absent.
  static QString parseLatestVersion(const QByteArray &p_json);
  // True if `latest` is a higher semantic version than `current` (numeric compare).
  static bool isNewer(const QString &p_latest, const QString &p_current);

  // Whether an auto-check is due (#20c). last==0 means never checked -> true; last<0 -> false.
  static bool shouldCheck(qint64 p_lastMs, qint64 p_nowMs, int p_intervalDays);

  // Async: GET https://api.github.com/repos/<owner>/<repo>/releases/latest.
  void check(const QString &p_owner, const QString &p_repo, const QString &p_currentVersion);

signals:
  void result(bool p_hasUpdate, const QString &p_latest, const QString &p_error);

private:
  QNetworkAccessManager m_nam;
};
} // namespace markly

#endif // MARKLY_UPDATECHECKER_H
