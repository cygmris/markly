#include <QFile>
#include <QTemporaryDir>
#include <QVariantMap>
#include <QtTest>

#include <core/configmgr.h>
#include <core/notebookmgr.h>
#include <widgets/explorer/notebookexplorer.h>

using namespace markly;

class TestNotebookExplorer : public QObject {
  Q_OBJECT
private:
  static QVariantMap findByName(const QVariantList &p_list, const QString &p_name) {
    for (const auto &v : p_list) {
      const auto m = v.toMap();
      if (m.value("name").toString() == p_name) {
        return m;
      }
    }
    return QVariantMap();
  }

private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }

  void testTreeFlattenAndCrud() {
    QTemporaryDir dir;
    NotebookMgr mgr;
    auto nb = mgr.newNotebook(dir.path(), QStringLiteral("nb"), QString());
    QVERIFY(nb);

    NotebookExplorer exp(&mgr);
    QVERIFY(exp.hasNotebook());
    QCOMPARE(exp.currentNotebookName(), QStringLiteral("nb"));
    QVERIFY(exp.visibleNodes().isEmpty()); // empty notebook

    // Create a folder at root.
    QCOMPARE(exp.newFolder(0, QStringLiteral("算法")), QString());
    auto vis = exp.visibleNodes();
    QCOMPARE(vis.size(), 1);
    auto folder = findByName(vis, QStringLiteral("算法"));
    QCOMPARE(folder.value("type").toString(), QStringLiteral("folder"));
    const double folderId = folder.value("nodeId").toDouble();

    // New note inside the folder (folder auto-expands).
    QCOMPARE(exp.newNote(folderId, QStringLiteral("a.md")), QString());
    vis = exp.visibleNodes();
    QCOMPARE(vis.size(), 2); // folder + note
    auto note = findByName(vis, QStringLiteral("a.md"));
    QCOMPARE(note.value("type").toString(), QStringLiteral("file"));
    QCOMPARE(note.value("depth").toInt(), 1);

    // Collapse the folder -> note hidden.
    exp.toggleExpand(folderId);
    QCOMPARE(exp.visibleNodes().size(), 1);
    exp.toggleExpand(folderId);
    QCOMPARE(exp.visibleNodes().size(), 2);

    // Color.
    const double noteId = findByName(exp.visibleNodes(), QStringLiteral("a.md"))
                              .value("nodeId").toDouble();
    exp.setNodeColor(noteId, QString(), QString(), QStringLiteral("#ff0000"));
    QCOMPARE(findByName(exp.visibleNodes(), QStringLiteral("a.md")).value("nameColor").toString(),
             QStringLiteral("#ff0000"));

    // Rename.
    QCOMPARE(exp.renameNode(noteId, QStringLiteral("b.md")), QString());
    QVERIFY(!findByName(exp.visibleNodes(), QStringLiteral("b.md")).isEmpty());

    // Invalid name rejected.
    QVERIFY(!exp.newNote(0, QString()).isEmpty());

    // Recycle.
    const double bId = findByName(exp.visibleNodes(), QStringLiteral("b.md"))
                           .value("nodeId").toDouble();
    exp.removeToRecycle(bId);
    QVERIFY(findByName(exp.visibleNodes(), QStringLiteral("b.md")).isEmpty());
  }

  void testExternalFilesAndImport() {
    QTemporaryDir dir;
    NotebookMgr mgr;
    auto nb = mgr.newNotebook(dir.path(), QStringLiteral("nb"), QString());
    QVERIFY(nb);

    // Drop an untracked file in the notebook root.
    {
      QFile f(QDir(dir.path()).filePath(QStringLiteral("external.md")));
      f.open(QIODevice::WriteOnly);
      f.write("# external");
    }

    NotebookExplorer exp(&mgr);
    // Trigger a rebuild (any op recomputes external).
    exp.newFolder(0, QStringLiteral("tmp"));
    auto ext = findByName(exp.visibleNodes(), QStringLiteral("external.md"));
    QVERIFY(!ext.isEmpty());
    QCOMPARE(ext.value("isExternal").toBool(), true);

    // Import it.
    exp.importExternal(0, QStringLiteral("external.md"));
    auto imported = findByName(exp.visibleNodes(), QStringLiteral("external.md"));
    QVERIFY(!imported.isEmpty());
    QCOMPARE(imported.value("isExternal").toBool(), false);
    QVERIFY(imported.value("nodeId").toDouble() > 0);
  }
};

QTEST_GUILESS_MAIN(TestNotebookExplorer)
#include "test_notebookexplorer.moc"
