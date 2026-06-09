#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QtTest>

#include <core/configmgr.h>
#include <core/task/taskmgr.h>

using namespace markly;

class TestTask : public QObject {
  Q_OBJECT
private:
  void writeTask(const QString &name, const QString &command) {
    const auto dir =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/tasks";
    QDir().mkpath(dir);
    QFile f(dir + "/" + name + ".json");
    f.open(QIODevice::WriteOnly);
    f.write(("{\"command\":\"" + command + "\",\"shell\":true}").toUtf8());
    f.close();
  }

private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }

  void testExpand() {
    TaskContext ctx;
    ctx.m_bufferName = QStringLiteral("note.md");
    ctx.m_bufferDir = QStringLiteral("/nb/folder");
    ctx.m_bufferPath = QStringLiteral("/nb/folder/note.md");
    ctx.m_notebookFolder = QStringLiteral("/nb");
    const auto out = TaskMgr::expand(
        QStringLiteral("${bufferBaseName} ${notebookFolder} ${unknown}"), ctx);
    QCOMPARE(out, QStringLiteral("note /nb ${unknown}"));
  }

  void testRunEcho() {
    writeTask(QStringLiteral("echo-base"), QStringLiteral("echo ${bufferBaseName}"));
    TaskMgr mgr;
    mgr.load();
    TaskContext ctx;
    ctx.m_bufferName = QStringLiteral("hello.md");
    int exitCode = -1;
    const auto out = mgr.run(QStringLiteral("echo-base"), ctx, exitCode);
    QCOMPARE(exitCode, 0);
    QVERIFY(out.contains(QStringLiteral("hello")));
  }

  void testRunMissingCommand() {
    writeTask(QStringLiteral("nope"), QStringLiteral("definitely_not_a_real_command_xyz"));
    TaskMgr mgr;
    mgr.load();
    TaskContext ctx;
    int exitCode = 0;
    const auto out = mgr.run(QStringLiteral("nope"), ctx, exitCode);
    QVERIFY(!out.isEmpty()); // sh reports "command not found"; no crash
  }

  void testSeedDefaults() {
    // A fresh config dir gets a seeded task on first load.
    TaskMgr mgr;
    mgr.load();
    QVERIFY(mgr.tasks().size() >= 1);
  }
};

QTEST_GUILESS_MAIN(TestTask)
#include "test_task.moc"
