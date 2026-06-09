#include "updatechecker.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QVersionNumber>

using namespace markly;

UpdateChecker::UpdateChecker(QObject *p_parent) : QObject(p_parent) {}

QString UpdateChecker::parseLatestVersion(const QByteArray &p_json) {
  const auto doc = QJsonDocument::fromJson(p_json);
  if (!doc.isObject()) {
    return QString();
  }
  QString tag = doc.object().value(QStringLiteral("tag_name")).toString();
  if (tag.startsWith(QLatin1Char('v')) || tag.startsWith(QLatin1Char('V'))) {
    tag = tag.mid(1);
  }
  return tag;
}

bool UpdateChecker::isNewer(const QString &p_latest, const QString &p_current) {
  if (p_latest.isEmpty()) {
    return false;
  }
  const auto a = QVersionNumber::fromString(p_latest);
  const auto b = QVersionNumber::fromString(p_current);
  return QVersionNumber::compare(a, b) > 0;
}

bool UpdateChecker::shouldCheck(qint64 p_lastMs, qint64 p_nowMs, int p_intervalDays) {
  if (p_lastMs == 0) {
    return true; // never checked
  }
  if (p_lastMs < 0) {
    return false;
  }
  return (p_nowMs - p_lastMs) >= static_cast<qint64>(p_intervalDays) * 86400000LL;
}

void UpdateChecker::check(const QString &p_owner, const QString &p_repo,
                          const QString &p_currentVersion) {
  QNetworkRequest req((QUrl(QStringLiteral("https://api.github.com/repos/%1/%2/releases/latest")
                                .arg(p_owner, p_repo))));
  req.setRawHeader("Accept", "application/vnd.github+json");

  QNetworkReply *reply = m_nam.get(req);
  connect(reply, &QNetworkReply::finished, this, [this, reply, p_currentVersion]() {
    const QByteArray body = reply->readAll();
    const auto err = reply->error();
    const QString errStr = reply->errorString();
    reply->deleteLater();
    if (err != QNetworkReply::NoError) {
      emit result(false, QString(), errStr);
      return;
    }
    const QString latest = parseLatestVersion(body);
    emit result(isNewer(latest, p_currentVersion), latest, QString());
  });
}
