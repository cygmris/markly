#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QtTest>

#include <core/notebook/node.h>
#include <core/notebook/notebook.h>
#include <core/notebook/notebookdatabaseaccess.h>
#include <core/notebookbackend/localnotebookbackend.h>

using namespace markly;

class TestNotebookCore : public QObject {
  Q_OBJECT
private:
  QJsonObject readJson(const QString &p_path) {
    QFile f(p_path);
    f.open(QIODevice::ReadOnly);
    return QJsonDocument::fromJson(f.readAll()).object();
  }

private slots:
  void testCreateNotebook() {
    QTemporaryDir dir;
    auto backend = new LocalNotebookBackend(dir.path());
    Notebook nb(1, dir.path(), backend);
    nb.createOnDisk(QStringLiteral("我的笔记本"), QStringLiteral("desc"));

    // vx_notebook.json present + fields.
    const auto cfgPath = dir.filePath(QStringLiteral("vx_notebook.json"));
    QVERIFY(QFileInfo::exists(cfgPath));
    auto cfg = readJson(cfgPath);
    QCOMPARE(cfg.value("version").toInt(), 3);
    QCOMPARE(cfg.value("name").toString(), QStringLiteral("我的笔记本"));
    QCOMPARE(cfg.value("image_folder").toString(), QStringLiteral("vx_images"));
    QCOMPARE(cfg.value("version_controller").toString(), QStringLiteral("dummy.vnotex"));

    // root vx.json present.
    QVERIFY(QFileInfo::exists(dir.filePath(QStringLiteral("vx.json"))));
  }

  void testCreateFolderAndNoteThreeWayConsistency() {
    QTemporaryDir dir;
    auto backend = new LocalNotebookBackend(dir.path());
    Notebook nb(2, dir.path(), backend);
    nb.createOnDisk(QStringLiteral("nb"), QString());

    auto root = nb.getRootNode();
    auto folder = nb.newNode(root, Node::Type::Folder, QStringLiteral("算法"));
    auto note = nb.newNode(folder, Node::Type::File, QStringLiteral("note.md"));

    // Disk: folder dir + its vx.json, file exists.
    QVERIFY(QFileInfo::exists(dir.filePath(QStringLiteral("算法"))));
    QVERIFY(QFileInfo::exists(dir.filePath(QStringLiteral("算法/vx.json"))));
    QVERIFY(QFileInfo::exists(dir.filePath(QStringLiteral("算法/note.md"))));

    // root vx.json lists the folder; folder vx.json lists the file.
    auto rootCfg = readJson(dir.filePath(QStringLiteral("vx.json")));
    QCOMPARE(rootCfg.value("folders").toArray().size(), 1);
    QCOMPARE(rootCfg.value("folders").toArray().at(0).toObject().value("name").toString(),
             QStringLiteral("算法"));
    auto folderCfg = readJson(dir.filePath(QStringLiteral("算法/vx.json")));
    QCOMPARE(folderCfg.value("files").toArray().size(), 1);
    QCOMPARE(folderCfg.value("files").toArray().at(0).toObject().value("name").toString(),
             QStringLiteral("note.md"));

    // DB: node rows for folder + note with correct parent linkage.
    auto *db = nb.getDatabase();
    QVERIFY(db->nodeExists(folder->getId()));
    QVERIFY(db->nodeExists(note->getId()));
    auto rootChildren = db->queryChildren(root->getId());
    QCOMPARE(rootChildren.size(), 1);
    QCOMPARE(rootChildren.at(0).m_id, folder->getId());
    auto folderChildren = db->queryChildren(folder->getId());
    QCOMPARE(folderChildren.size(), 1);
    QCOMPARE(folderChildren.at(0).m_id, note->getId());
  }

  void testRenameMoveToRecycleAndColor() {
    QTemporaryDir dir;
    auto backend = new LocalNotebookBackend(dir.path());
    Notebook nb(3, dir.path(), backend);
    nb.createOnDisk(QStringLiteral("nb"), QString());
    auto root = nb.getRootNode();
    auto note = nb.newNode(root, Node::Type::File, QStringLiteral("a.md"));

    // Rename.
    nb.renameNode(note, QStringLiteral("b.md"));
    QVERIFY(!QFileInfo::exists(dir.filePath(QStringLiteral("a.md"))));
    QVERIFY(QFileInfo::exists(dir.filePath(QStringLiteral("b.md"))));
    auto rootCfg = readJson(dir.filePath(QStringLiteral("vx.json")));
    QCOMPARE(rootCfg.value("files").toArray().at(0).toObject().value("name").toString(),
             QStringLiteral("b.md"));

    // Color customization writes to parent's files[] entry.
    nb.updateNodeVisual(note, NodeVisual(QStringLiteral("#ff0000"), QString(), QString()));
    rootCfg = readJson(dir.filePath(QStringLiteral("vx.json")));
    QCOMPARE(
        rootCfg.value("files").toArray().at(0).toObject().value("background_color").toString(),
        QStringLiteral("#ff0000"));

    // Move to recycle bin.
    const auto noteId = note->getId();
    nb.moveNodeToRecycleBin(note);
    QVERIFY(!QFileInfo::exists(dir.filePath(QStringLiteral("b.md"))));
    QVERIFY(!nb.getDatabase()->nodeExists(noteId));
    QCOMPARE(nb.getRootNode()->getChildrenCount(), 0);
    // Recycle bin folder created.
    QVERIFY(QDir(dir.filePath(QStringLiteral("vx_recycle_bin"))).exists());
  }

  void testReopenConsistency() {
    QTemporaryDir dir;
    {
      auto backend = new LocalNotebookBackend(dir.path());
      Notebook nb(4, dir.path(), backend);
      nb.createOnDisk(QStringLiteral("nb"), QString());
      auto root = nb.getRootNode();
      auto folder = nb.newNode(root, Node::Type::Folder, QStringLiteral("f1"));
      nb.newNode(folder, Node::Type::File, QStringLiteral("n1.md"));
    }
    // Reopen.
    auto backend = new LocalNotebookBackend(dir.path());
    Notebook nb(5, dir.path(), backend);
    nb.open();
    auto root = nb.getRootNode();
    nb.loadNodeChildren(root);
    QCOMPARE(root->getChildrenCount(), 1);
    auto folder = root->getChildren().at(0);
    QCOMPARE(folder->getName(), QStringLiteral("f1"));
    nb.loadNodeChildren(folder);
    QCOMPARE(folder->getChildrenCount(), 1);
    QCOMPARE(folder->getChildren().at(0)->getName(), QStringLiteral("n1.md"));
  }

  void testVNoteCompatRead() {
    // Hand-build a minimal VNote v3 notebook and open it.
    QTemporaryDir dir;
    QDir d(dir.path());

    QJsonObject nbCfg;
    nbCfg["version"] = 3;
    nbCfg["name"] = QStringLiteral("VNoteNB");
    nbCfg["image_folder"] = QStringLiteral("vx_images");
    nbCfg["attachment_folder"] = QStringLiteral("vx_attachments");
    nbCfg["version_controller"] = QStringLiteral("dummy.vnotex");
    nbCfg["config_mgr"] = QStringLiteral("vx.vnotex");
    {
      QFile f(d.filePath(QStringLiteral("vx_notebook.json")));
      f.open(QIODevice::WriteOnly);
      f.write(QJsonDocument(nbCfg).toJson());
    }

    QJsonObject fileEntry;
    fileEntry["name"] = QStringLiteral("welcome.md");
    fileEntry["id"] = 2;
    fileEntry["signature"] = 999;
    fileEntry["tags"] = QJsonArray{QStringLiteral("intro")};
    QJsonObject rootCfg;
    rootCfg["version"] = 1;
    rootCfg["id"] = 1;
    rootCfg["signature"] = 111;
    rootCfg["files"] = QJsonArray{fileEntry};
    rootCfg["folders"] = QJsonArray{};
    {
      QFile f(d.filePath(QStringLiteral("vx.json")));
      f.open(QIODevice::WriteOnly);
      f.write(QJsonDocument(rootCfg).toJson());
    }
    {
      QFile f(d.filePath(QStringLiteral("welcome.md")));
      f.open(QIODevice::WriteOnly);
      f.write("# Welcome");
    }

    auto backend = new LocalNotebookBackend(dir.path());
    Notebook nb(6, dir.path(), backend);
    nb.open();
    QCOMPARE(nb.getName(), QStringLiteral("VNoteNB"));
    auto root = nb.getRootNode();
    nb.loadNodeChildren(root);
    QCOMPARE(root->getChildrenCount(), 1);
    auto note = root->getChildren().at(0);
    QCOMPARE(note->getName(), QStringLiteral("welcome.md"));
    QCOMPARE(note->getId(), static_cast<ID>(2));
    QVERIFY(note->getTags().contains(QStringLiteral("intro")));
    // DB index rebuilt from disk.
    QVERIFY(nb.getDatabase()->nodeExists(2));
  }
};

QTEST_GUILESS_MAIN(TestNotebookCore)
#include "test_notebookcore.moc"
