#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

#include <widgets/export/exporthelper.h>

using namespace markly;

class TestExport : public QObject {
  Q_OBJECT
private slots:
  void testWriteText() {
    QTemporaryDir dir;
    const auto path = dir.filePath(QStringLiteral("out.md"));
    ExportHelper helper;
    const auto content = QStringLiteral("# 标题\n\n正文内容。\n");
    QVERIFY(helper.writeText(path, content));
    QFile f(path);
    QVERIFY(f.open(QIODevice::ReadOnly));
    QCOMPARE(QString::fromUtf8(f.readAll()), content);
  }

  void testWriteTextBadPath() {
    ExportHelper helper;
    QVERIFY(!helper.writeText(QStringLiteral("/no/such/dir/x.md"), QStringLiteral("x")));
    QVERIFY(!helper.writeText(QString(), QStringLiteral("x")));
  }
};

QTEST_GUILESS_MAIN(TestExport)
#include "test_export.moc"
