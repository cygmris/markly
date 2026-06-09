#include <QtTest>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include <core/export/pandocexporter.h>

using namespace markly;

// Tests for Pandoc export (#15b). buildArgs is pure; the conversion runs real pandoc
// when available (guarded), so it is a genuine integration test.
class TestPandoc : public QObject {
  Q_OBJECT
private slots:
  void buildArgs();
  void realConversion();
  void optionArgs();
  void tocConversion();
};

void TestPandoc::buildArgs() {
  const QStringList a = PandocExporter::buildArgs(QStringLiteral("a.md"),
                                                  QStringLiteral("b.docx"),
                                                  QStringLiteral("docx"));
  QCOMPARE(a, (QStringList{"-f", "markdown", "-t", "docx", "-o", "b.docx", "a.md"}));
}

void TestPandoc::realConversion() {
  if (!PandocExporter::isAvailable()) {
    QSKIP("pandoc not installed");
  }
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const QString out = dir.filePath(QStringLiteral("out.html"));
  QString err;
  const bool ok = PandocExporter::exportTo(QStringLiteral("# Title\n\nHello world"), out,
                                           QStringLiteral("html"), &err);
  QVERIFY2(ok, qPrintable(err));

  QFile f(out);
  QVERIFY(f.open(QIODevice::ReadOnly));
  const QString html = QString::fromUtf8(f.readAll());
  QVERIFY(html.contains(QStringLiteral("<h1")));
  QVERIFY(html.contains(QStringLiteral("Title")));

  // Failure path: an unwritable output directory fails (not crashes).
  QString err2;
  QVERIFY(!PandocExporter::exportTo(QStringLiteral("# x"),
                                    QStringLiteral("/no/such/dir/out.html"),
                                    QStringLiteral("html"), &err2));
}

void TestPandoc::optionArgs() {
  PandocOptions o;
  o.standalone = true;
  o.toc = true;
  o.cssPath = QStringLiteral("a.css");
  const QStringList a = PandocExporter::buildArgs(QStringLiteral("in.md"), QStringLiteral("out.html"),
                                                  QStringLiteral("html"), QStringLiteral("markdown"), o);
  QVERIFY(a.contains(QStringLiteral("--standalone")));
  QVERIFY(a.contains(QStringLiteral("--toc")));
  QVERIFY(a.contains(QStringLiteral("--css")));
  QVERIFY(a.contains(QStringLiteral("a.css")));

  // No options -> equivalent to the base args (no extra flags).
  const QStringList b = PandocExporter::buildArgs(QStringLiteral("in.md"), QStringLiteral("out.html"),
                                                  QStringLiteral("html"), QStringLiteral("markdown"),
                                                  PandocOptions{});
  QVERIFY(!b.contains(QStringLiteral("--standalone")));
  QVERIFY(!b.contains(QStringLiteral("--toc")));
}

void TestPandoc::tocConversion() {
  if (!PandocExporter::isAvailable()) {
    QSKIP("pandoc not installed");
  }
  QTemporaryDir dir;
  const QString out = dir.filePath(QStringLiteral("out.html"));
  PandocOptions o;
  o.standalone = true;
  o.toc = true;
  QString err;
  QVERIFY2(PandocExporter::exportTo(QStringLiteral("# A\n\n## B\n\n## C"), out,
                                    QStringLiteral("html"), o, &err),
           qPrintable(err));
  QFile f(out);
  QVERIFY(f.open(QIODevice::ReadOnly));
  const QString html = QString::fromUtf8(f.readAll());
  QVERIFY(html.contains(QStringLiteral("id=\"TOC\"")));
}

QTEST_GUILESS_MAIN(TestPandoc)
#include "test_pandoc.moc"
