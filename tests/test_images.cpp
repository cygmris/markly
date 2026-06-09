#include <QFile>
#include <QImage>
#include <QTemporaryDir>
#include <QtTest>

#include <widgets/images/imagehelper.h>

using namespace markly;

class TestImages : public QObject {
  Q_OBJECT
private slots:
  void testImportImageCopiesAndReturnsRel() {
    QTemporaryDir srcDir, noteDir;
    // Make a 1x1 PNG source.
    const auto src = srcDir.filePath(QStringLiteral("pic.png"));
    QImage img(1, 1, QImage::Format_RGB32);
    img.fill(Qt::red);
    QVERIFY(img.save(src, "PNG"));

    ImageHelper helper;
    const auto rel = helper.importImage(src, noteDir.path());
    QVERIFY(rel.startsWith(QStringLiteral("vx_images/img_")));
    QVERIFY(rel.endsWith(QStringLiteral(".png")));
    // Copied file exists under noteDir/vx_images.
    QVERIFY(QFile::exists(noteDir.filePath(rel)));
  }

  void testImportImageRejectsNonImage() {
    QTemporaryDir srcDir, noteDir;
    const auto src = srcDir.filePath(QStringLiteral("note.txt"));
    QFile f(src);
    f.open(QIODevice::WriteOnly);
    f.write("hello");
    f.close();

    ImageHelper helper;
    QCOMPARE(helper.importImage(src, noteDir.path()), QString());
  }

  void testImportImageFileUrl() {
    QTemporaryDir srcDir, noteDir;
    const auto src = srcDir.filePath(QStringLiteral("a.jpg"));
    QImage img(2, 2, QImage::Format_RGB32);
    img.fill(Qt::blue);
    QVERIFY(img.save(src, "JPG"));

    ImageHelper helper;
    const auto rel = helper.importImage(QStringLiteral("file://") + src, noteDir.path());
    QVERIFY(rel.startsWith(QStringLiteral("vx_images/img_")));
    QVERIFY(rel.endsWith(QStringLiteral(".jpg")));
    QVERIFY(QFile::exists(noteDir.filePath(rel)));
  }
};

QTEST_GUILESS_MAIN(TestImages)
#include "test_images.moc"
