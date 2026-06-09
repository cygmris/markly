#include <QtTest>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include <core/export/notemerge.h>
#include <core/export/pandocexporter.h>

using namespace markly;

// Tests for merged folder export (#15c). merge/mergeDir are pure; the full export runs
// real pandoc when available (guarded).
class TestMerge : public QObject {
  Q_OBJECT
private slots:
  void mergeNotes();
  void mergeDirectory();
  void exportMerged();

private:
  static void writeFile(const QString &path, const QString &content) {
    QFile f(path);
    f.open(QIODevice::WriteOnly);
    f.write(content.toUtf8());
    f.close();
  }
};

void TestMerge::mergeNotes() {
  QVector<MergeNote> notes{{QStringLiteral("A"), QStringLiteral("x")},
                           {QStringLiteral("B"), QStringLiteral("y")}};
  const QString m = NoteMerge::merge(notes, true);
  QVERIFY(m.contains(QStringLiteral("# A")));
  QVERIFY(m.contains(QStringLiteral("x")));
  QVERIFY(m.contains(QStringLiteral("---")));
  QVERIFY(m.contains(QStringLiteral("# B")));
  QVERIFY(m.contains(QStringLiteral("y")));
}

void TestMerge::mergeDirectory() {
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  writeFile(dir.filePath(QStringLiteral("1.md")), QStringLiteral("# one\nAlpha"));
  writeFile(dir.filePath(QStringLiteral("2.md")), QStringLiteral("# two\nBeta"));
  const QString m = NoteMerge::mergeDir(dir.path());
  QVERIFY(m.contains(QStringLiteral("Alpha")));
  QVERIFY(m.contains(QStringLiteral("Beta")));
  // Sorted by name: 1.md before 2.md.
  QVERIFY(m.indexOf(QStringLiteral("Alpha")) < m.indexOf(QStringLiteral("Beta")));

  QTemporaryDir empty;
  QCOMPARE(NoteMerge::mergeDir(empty.path()), QString());
}

void TestMerge::exportMerged() {
  QTemporaryDir dir;
  writeFile(dir.filePath(QStringLiteral("a.md")), QStringLiteral("Alpha content"));
  writeFile(dir.filePath(QStringLiteral("b.md")), QStringLiteral("Beta content"));

  // md format: direct write.
  const QString mdOut = dir.filePath(QStringLiteral("out.md"));
  QVERIFY(NoteMerge::exportMergedDir(dir.path(), mdOut, QStringLiteral("md")));
  QVERIFY(QFile::exists(mdOut));

  if (!PandocExporter::isAvailable()) {
    QSKIP("pandoc not installed");
  }
  const QString htmlOut = dir.filePath(QStringLiteral("out.html"));
  QString err;
  QVERIFY2(NoteMerge::exportMergedDir(dir.path(), htmlOut, QStringLiteral("html"), &err),
           qPrintable(err));
  QFile f(htmlOut);
  QVERIFY(f.open(QIODevice::ReadOnly));
  const QString html = QString::fromUtf8(f.readAll());
  QVERIFY(html.contains(QStringLiteral("Alpha")));
  QVERIFY(html.contains(QStringLiteral("Beta")));
}

QTEST_GUILESS_MAIN(TestMerge)
#include "test_merge.moc"
