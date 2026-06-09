#include <QTemporaryDir>
#include <QtTest>

#include <core/configmgr.h>
#include <core/notebook/node.h>
#include <core/notebook/notebook.h>
#include <core/notebook/notebookdatabaseaccess.h>
#include <core/notebook/notebooktagmgr.h>
#include <core/notebookbackend/localnotebookbackend.h>
#include <core/notebookmgr.h>

#include <widgets/tags/tagbridge.h>

using namespace markly;

class TestTags : public QObject {
  Q_OBJECT
private:
  int tagCount(const QVariantList &tags, const QString &name) {
    for (const auto &v : tags) {
      const auto m = v.toMap();
      if (m.value(QStringLiteral("name")).toString() == name) {
        return m.value(QStringLiteral("count")).toInt();
      }
    }
    return -1;
  }

private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }

  void testTaggingAndBridge() {
    QTemporaryDir dir;
    {
      auto *backend = new LocalNotebookBackend(dir.path());
      Notebook nb(1, dir.path(), backend);
      nb.createOnDisk(QStringLiteral("nb"), QString());
      auto root = nb.getRootNode();
      auto n1 = nb.newNode(root, Node::Type::File, QStringLiteral("a.md"));
      auto n2 = nb.newNode(root, Node::Type::File, QStringLiteral("b.md"));
      nb.updateNodeTags(n1, {QStringLiteral("算法"), QStringLiteral("DP")});
      nb.updateNodeTags(n2, {QStringLiteral("算法")});

      // DB index reflects the tags.
      QCOMPARE(nb.getDatabase()->queryNodesByTag(QStringLiteral("算法")).size(), 2);
      QCOMPARE(nb.getDatabase()->queryNodesByTag(QStringLiteral("DP")).size(), 1);
      QVERIFY(nb.getTagMgr()->getTags().contains(QStringLiteral("算法")));
      QVERIFY(nb.getTagMgr()->getTags().contains(QStringLiteral("DP")));
    }

    NotebookMgr mgr;
    auto nb = mgr.openNotebook(dir.path());
    QVERIFY(nb);
    mgr.setCurrentNotebook(nb->getId());

    TagBridge bridge(&mgr);
    bridge.refresh();
    QCOMPARE(tagCount(bridge.tags(), QStringLiteral("算法")), 2);
    QCOMPARE(tagCount(bridge.tags(), QStringLiteral("DP")), 1);

    bridge.selectTag(QStringLiteral("算法"));
    QCOMPARE(bridge.taggedNodes().size(), 2);
    bridge.selectTag(QStringLiteral("DP"));
    QCOMPARE(bridge.taggedNodes().size(), 1);

    // Clear tags on the DP-bearing node.
    QSharedPointer<Node> dpNode;
    for (const auto &n : nb->collectMarkdownNodes()) {
      if (n->getTags().contains(QStringLiteral("DP"))) {
        dpNode = n;
        break;
      }
    }
    QVERIFY(dpNode);
    nb->updateNodeTags(dpNode, {});
    QCOMPARE(nb->getDatabase()->queryNodesByTag(QStringLiteral("DP")).size(), 0);
  }
};

QTEST_GUILESS_MAIN(TestTags)
#include "test_tags.moc"
