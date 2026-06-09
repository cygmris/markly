#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QVariantMap>
#include <QtTest>

#include <core/buffer/buffer.h>
#include <core/buffer/buffermgr.h>
#include <core/configmgr.h>
#include <core/sessionconfig.h>
#include <widgets/viewarea/viewarea.h>

using namespace markly;

class TestBufferView : public QObject {
  Q_OBJECT
private:
  QString writeTemp(QTemporaryDir &d, const QString &name, const QString &content) {
    const auto path = d.filePath(name);
    QFile f(path);
    f.open(QIODevice::WriteOnly);
    f.write(content.toUtf8());
    f.close();
    return path;
  }

private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }

  void testBufferMgrDedupAndSave() {
    QTemporaryDir dir;
    const auto path = writeTemp(dir, "a.md", "# Hello");
    BufferMgr mgr;
    auto *b1 = mgr.open(path);
    QVERIFY(b1);
    QCOMPARE(b1->getContent(), QStringLiteral("# Hello"));
    QVERIFY(!b1->isDirty());

    // Reopen same path -> same buffer.
    auto *b2 = mgr.open(path);
    QCOMPARE(b1, b2);

    b1->setContent(QStringLiteral("# Changed"));
    QVERIFY(b1->isDirty());
    b1->save();
    QVERIFY(!b1->isDirty());
    QFile f(path);
    f.open(QIODevice::ReadOnly);
    QCOMPARE(QString::fromUtf8(f.readAll()), QStringLiteral("# Changed"));
  }

  void testViewAreaTabsAndEdit() {
    QTemporaryDir dir;
    const auto p1 = writeTemp(dir, "a.md", "AAA");
    const auto p2 = writeTemp(dir, "b.md", "BBB");
    BufferMgr mgr;
    ViewArea va(&mgr);

    QVERIFY(!va.hasOpenFile());
    va.openFile(p1);
    QVERIFY(va.hasOpenFile());
    auto splits = va.splits();
    QCOMPARE(splits.size(), 1);
    auto s0 = splits.at(0).toMap();
    QCOMPARE(s0.value("tabs").toList().size(), 1);
    QCOMPARE(s0.value("currentText").toString(), QStringLiteral("AAA"));
    QCOMPARE(s0.value("currentPath").toString(), p1); // viewer (#18) uses this path

    va.openFile(p2);
    s0 = va.splits().at(0).toMap();
    QCOMPARE(s0.value("tabs").toList().size(), 2);
    QCOMPARE(s0.value("currentText").toString(), QStringLiteral("BBB"));

    const double bufB = s0.value("currentBufferId").toDouble();
    va.updateText(bufB, QStringLiteral("BBB edited"));
    QVERIFY(va.isTabDirty(bufB));
    va.saveTab(bufB);
    QVERIFY(!va.isTabDirty(bufB));

    // Activate first tab.
    const double bufA = va.splits().at(0).toMap().value("tabs").toList().at(0).toMap()
                            .value("bufferId").toDouble();
    va.activateTab(0, bufA);
    QCOMPARE(va.splits().at(0).toMap().value("currentText").toString(), QStringLiteral("AAA"));

    // Split.
    va.splitView();
    QCOMPARE(va.splitCount(), 2);

    // Session persisted opened files.
    const auto opened = ConfigMgr::getInst().getSessionConfig().getOpenedFiles();
    QVERIFY(opened.contains(p1));
    QVERIFY(opened.contains(p2));

    // Close a tab.
    va.closeTab(0, bufA);
    // bufA still referenced? only split 0 had it; now removed.
    QVERIFY(!va.isTabDirty(bufA)); // closed/no buffer -> false
  }

  void testOutlineParsing() {
    QTemporaryDir dir;
    const auto p = writeTemp(dir, "doc.md",
                             "# Title\n\nintro\n\n## Section A\n\n```\n# not a heading\n```\n\n"
                             "## Section B\n\n### Sub\n");
    BufferMgr mgr;
    ViewArea va(&mgr);
    va.openFile(p);
    const auto outline = va.outline();
    QCOMPARE(outline.size(), 4); // Title, Section A, Section B, Sub (fence # excluded)
    QCOMPARE(outline.at(0).toMap().value("text").toString(), QStringLiteral("Title"));
    QCOMPARE(outline.at(0).toMap().value("level").toInt(), 1);
    QCOMPARE(outline.at(0).toMap().value("line").toInt(), 1);
    QCOMPARE(outline.at(1).toMap().value("text").toString(), QStringLiteral("Section A"));
    QCOMPARE(outline.at(3).toMap().value("text").toString(), QStringLiteral("Sub"));
    QCOMPARE(outline.at(3).toMap().value("level").toInt(), 3);

    // Update text -> outline reflects it.
    const double bid = va.splits().at(0).toMap().value("currentBufferId").toDouble();
    va.updateText(bid, QStringLiteral("# Only One\n"));
    QCOMPARE(va.outline().size(), 1);
  }

  void testViewModeCycle() {
    BufferMgr bm;
    ViewArea va(&bm);
    QCOMPARE(va.viewMode(), QStringLiteral("edit"));
    va.cycleViewMode();
    QCOMPARE(va.viewMode(), QStringLiteral("split"));
    va.cycleViewMode();
    QCOMPARE(va.viewMode(), QStringLiteral("read"));
    va.cycleViewMode();
    QCOMPARE(va.viewMode(), QStringLiteral("edit"));
    va.setViewMode(QStringLiteral("read"));
    QCOMPARE(va.viewMode(), QStringLiteral("read"));
  }
};

QTEST_GUILESS_MAIN(TestBufferView)
#include "test_bufferview.moc"
