#include <QtTest>

#include <application/commandlineoptions.h>

using namespace markly;

class TestCommandLineOptions : public QObject {
  Q_OBJECT
private slots:
  void testPathsAndSwitches() {
    CommandLineOptions opts;
    auto res = opts.parse(QStringList()
                          << "markly" << "--verbose" << "--log-stderr" << "--watch-themes"
                          << "a.md" << "/tmp/b.md");
    QCOMPARE(res, CommandLineOptions::Ok);
    QVERIFY(opts.m_verbose);
    QVERIFY(opts.m_logToStderr);
    QVERIFY(opts.m_watchThemes);
    QCOMPARE(opts.m_pathsToOpen.size(), 2);
    QCOMPARE(opts.m_pathsToOpen.at(0), QStringLiteral("a.md"));
    QCOMPARE(opts.m_pathsToOpen.at(1), QStringLiteral("/tmp/b.md"));
  }

  void testDefaults() {
    CommandLineOptions opts;
    auto res = opts.parse(QStringList() << "markly");
    QCOMPARE(res, CommandLineOptions::Ok);
    QVERIFY(!opts.m_verbose);
    QVERIFY(!opts.m_logToStderr);
    QVERIFY(!opts.m_watchThemes);
    QVERIFY(opts.m_pathsToOpen.isEmpty());
  }

  void testVersionRequested() {
    CommandLineOptions opts;
    QCOMPARE(opts.parse(QStringList() << "markly" << "--version"),
             CommandLineOptions::VersionRequested);
  }

  void testHelpRequested() {
    CommandLineOptions opts;
    QCOMPARE(opts.parse(QStringList() << "markly" << "--help"),
             CommandLineOptions::HelpRequested);
    QVERIFY(!opts.m_helpText.isEmpty());
  }

  void testErrorOnUnknownOption() {
    CommandLineOptions opts;
    QCOMPARE(opts.parse(QStringList() << "markly" << "--definitely-not-an-option"),
             CommandLineOptions::Error);
    QVERIFY(!opts.m_errorMsg.isEmpty());
  }
};

QTEST_GUILESS_MAIN(TestCommandLineOptions)
#include "test_commandlineoptions.moc"
