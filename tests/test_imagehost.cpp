#include <QtTest>

#include <QJsonDocument>
#include <QJsonObject>

#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryDir>

#include <core/configmgr.h>
#include <core/imagehost/githubimagehost.h>
#include <core/imagehost/giteeimagehost.h>
#include <core/imagehost/repoimagehost.h>
#include <core/widgetconfig.h>

using namespace markly;

// GUILESS tests for the GitHub image host request/response builders (#10b) and the
// WidgetConfig image_host round-trip. No real network access.
class TestImageHost : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }
  void uploadUrl();
  void uploadBody();
  void authHeader();
  void parseDownloadUrl();
  void configRoundTrip();
  void giteeUploadUrl();
  void giteeUploadBody();
  void repoRawUrl();
  void repoCommit();
  void repoCloneHelpers();
};

void TestImageHost::uploadUrl() {
  QCOMPARE(GithubImageHost::uploadUrl(QStringLiteral("u"), QStringLiteral("r"),
                                      QStringLiteral("img/a.png")),
           QStringLiteral("https://api.github.com/repos/u/r/contents/img/a.png"));
}

void TestImageHost::uploadBody() {
  const QByteArray body =
      GithubImageHost::uploadBody(QStringLiteral("p.png"), QByteArrayLiteral("hi"),
                                  QStringLiteral("main"));
  const QJsonObject o = QJsonDocument::fromJson(body).object();
  QCOMPARE(o.value(QStringLiteral("content")).toString(), QStringLiteral("aGk=")); // base64("hi")
  QVERIFY(o.value(QStringLiteral("message")).toString().contains(QStringLiteral("p.png")));
  QCOMPARE(o.value(QStringLiteral("branch")).toString(), QStringLiteral("main"));

  // No branch key when branch is empty.
  const QByteArray body2 =
      GithubImageHost::uploadBody(QStringLiteral("p.png"), QByteArrayLiteral("hi"), QString());
  QVERIFY(!QJsonDocument::fromJson(body2).object().contains(QStringLiteral("branch")));
}

void TestImageHost::authHeader() {
  QCOMPARE(GithubImageHost::authHeader(QStringLiteral("T")), QStringLiteral("token T"));
}

void TestImageHost::parseDownloadUrl() {
  const QByteArray ok = R"({"content":{"download_url":"https://x/y.png"}})";
  QCOMPARE(GithubImageHost::parseDownloadUrl(ok), QStringLiteral("https://x/y.png"));
  QCOMPARE(GithubImageHost::parseDownloadUrl(QByteArrayLiteral("{}")), QString());
}

void TestImageHost::configRoundTrip() {
  auto &wc = ConfigMgr::getInst().getWidgetConfig();
  QJsonObject c;
  c[QStringLiteral("type")] = QStringLiteral("github");
  c[QStringLiteral("user")] = QStringLiteral("alice");
  c[QStringLiteral("repo")] = QStringLiteral("imgs");
  c[QStringLiteral("token")] = QStringLiteral("tok");
  c[QStringLiteral("branch")] = QStringLiteral("main");
  wc.setImageHostConfig(c);

  const QJsonObject got = wc.getImageHostConfig();
  QCOMPARE(got.value(QStringLiteral("user")).toString(), QStringLiteral("alice"));
  QCOMPARE(got.value(QStringLiteral("repo")).toString(), QStringLiteral("imgs"));
  QVERIFY(wc.toJson().contains(QStringLiteral("image_host")));
}

void TestImageHost::giteeUploadUrl() {
  QCOMPARE(GiteeImageHost::uploadUrl(QStringLiteral("u"), QStringLiteral("r"),
                                     QStringLiteral("a.png")),
           QStringLiteral("https://gitee.com/api/v5/repos/u/r/contents/a.png"));
}

void TestImageHost::giteeUploadBody() {
  const QByteArray body = GiteeImageHost::uploadBody(
      QStringLiteral("p.png"), QByteArrayLiteral("hi"), QStringLiteral("main"),
      QStringLiteral("TOK"));
  const QJsonObject o = QJsonDocument::fromJson(body).object();
  QCOMPARE(o.value(QStringLiteral("access_token")).toString(), QStringLiteral("TOK"));
  QCOMPARE(o.value(QStringLiteral("content")).toString(), QStringLiteral("aGk="));
  QVERIFY(o.value(QStringLiteral("message")).toString().contains(QStringLiteral("p.png")));
  QCOMPARE(o.value(QStringLiteral("branch")).toString(), QStringLiteral("main"));

  const QByteArray body2 = GiteeImageHost::uploadBody(
      QStringLiteral("p.png"), QByteArrayLiteral("hi"), QString(), QStringLiteral("TOK"));
  QVERIFY(!QJsonDocument::fromJson(body2).object().contains(QStringLiteral("branch")));
}

void TestImageHost::repoRawUrl() {
  QCOMPARE(RepoImageHost::rawUrl(QStringLiteral("https://raw/u/r"), QStringLiteral("main"),
                                 QStringLiteral("img/a.png")),
           QStringLiteral("https://raw/u/r/main/img/a.png"));
  // Trailing slash trimmed.
  QCOMPARE(RepoImageHost::rawUrl(QStringLiteral("https://raw/u/r/"), QStringLiteral("main"),
                                 QStringLiteral("a.png")),
           QStringLiteral("https://raw/u/r/main/a.png"));
}

void TestImageHost::repoCommit() {
  if (QStandardPaths::findExecutable(QStringLiteral("git")).isEmpty()) {
    QSKIP("git not installed");
  }
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  QProcess init;
  init.start(QStringLiteral("git"),
             {QStringLiteral("-C"), dir.path(), QStringLiteral("init"),
              QStringLiteral("-b"), QStringLiteral("main")});
  QVERIFY(init.waitForFinished(15000));
  QCOMPARE(init.exitCode(), 0);

  QString err;
  QVERIFY2(RepoImageHost::commitImage(dir.path(), QStringLiteral("img/a.png"),
                                      QByteArrayLiteral("\x89PNG-data"),
                                      QStringLiteral("add a"), &err),
           qPrintable(err));
  QVERIFY(QFile::exists(dir.filePath(QStringLiteral("img/a.png"))));

  // The commit exists and the file is tracked.
  QProcess log;
  log.start(QStringLiteral("git"), {QStringLiteral("-C"), dir.path(),
                                    QStringLiteral("log"), QStringLiteral("--oneline")});
  log.waitForFinished(15000);
  QVERIFY(!log.readAllStandardOutput().trimmed().isEmpty());

  QProcess ls;
  ls.start(QStringLiteral("git"), {QStringLiteral("-C"), dir.path(), QStringLiteral("ls-files")});
  ls.waitForFinished(15000);
  QVERIFY(QString::fromUtf8(ls.readAllStandardOutput()).contains(QStringLiteral("img/a.png")));

  // A non-repo directory fails (no crash).
  QTemporaryDir notRepo;
  QVERIFY(!RepoImageHost::commitImage(notRepo.path(), QStringLiteral("x.png"),
                                      QByteArrayLiteral("x"), QStringLiteral("m")));
}

void TestImageHost::repoCloneHelpers() {
  QCOMPARE(RepoImageHost::cloneArgs(QStringLiteral("https://x/r.git"), QStringLiteral("/tmp/r")),
           (QStringList{"clone", "https://x/r.git", "/tmp/r"}));

  if (QStandardPaths::findExecutable(QStringLiteral("git")).isEmpty()) {
    QSKIP("git not installed");
  }
  QTemporaryDir dir;
  QVERIFY(!RepoImageHost::isGitRepo(dir.path())); // no .git yet
  QProcess init;
  init.start(QStringLiteral("git"), {QStringLiteral("-C"), dir.path(), QStringLiteral("init")});
  init.waitForFinished(15000);
  QVERIFY(RepoImageHost::isGitRepo(dir.path()));

  // ensureCloned fast path: already a repo -> true without a clone URL (no network).
  QVERIFY(RepoImageHost::ensureCloned(QString(), dir.path()));
  // Not a repo and no clone URL -> false.
  QTemporaryDir empty;
  QVERIFY(!RepoImageHost::ensureCloned(QString(), empty.path()));
}

QTEST_GUILESS_MAIN(TestImageHost)
#include "test_imagehost.moc"
