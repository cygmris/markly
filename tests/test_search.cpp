#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

#include <core/configmgr.h>
#include <core/notebook/node.h>
#include <core/notebook/notebook.h>
#include <core/notebookbackend/localnotebookbackend.h>
#include <core/notebookmgr.h>
#include <core/search/searcher.h>

using namespace markly;

class TestSearch : public QObject {
  Q_OBJECT
private:
  // Create a note with content on disk under the given parent.
  QSharedPointer<Node> makeNote(Notebook &p_nb, const QSharedPointer<Node> &p_parent,
                                const QString &p_name, const QString &p_content) {
    auto note = p_nb.newNode(p_parent, Node::Type::File, p_name);
    QFile f(note->fetchAbsolutePath());
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    f.write(p_content.toUtf8());
    f.close();
    return note;
  }

private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }

  void testContentAndNameSearch() {
    QTemporaryDir dir;
    {
      auto *backend = new LocalNotebookBackend(dir.path());
      Notebook nb(1, dir.path(), backend);
      nb.createOnDisk(QStringLiteral("nb"), QString());
      auto root = nb.getRootNode();
      makeNote(nb, root, QStringLiteral("alpha-note.md"),
               QStringLiteral("# Heading\n\nThe quick alpha fox jumps."));
      makeNote(nb, root, QStringLiteral("other.md"),
               QStringLiteral("# Other\n\nNothing to see, just beta content."));
    }

    NotebookMgr mgr;
    auto nb = mgr.openNotebook(dir.path());
    QVERIFY(nb);
    mgr.setCurrentNotebook(nb->getId());

    Searcher searcher(&mgr);

    // Content search via FTS5.
    SearchOption opt;
    opt.m_keyword = QStringLiteral("alpha");
    opt.m_scope = SearchScope::CurrentNotebook;
    opt.m_objects = SearchObjects(ObjContent);
    auto res = searcher.search(opt);
    QCOMPARE(res.size(), 1);
    QCOMPARE(res.first().m_name, QStringLiteral("alpha-note.md"));
    QVERIFY(res.first().m_snippet.contains(QStringLiteral("<mark>")));

    // Name search (in-memory).
    SearchOption nameOpt;
    nameOpt.m_keyword = QStringLiteral("other");
    nameOpt.m_objects = SearchObjects(ObjName);
    auto nameRes = searcher.search(nameOpt);
    QCOMPARE(nameRes.size(), 1);
    QCOMPARE(nameRes.first().m_name, QStringLiteral("other.md"));

    // Empty keyword -> no results.
    SearchOption emptyOpt;
    emptyOpt.m_keyword = QStringLiteral("   ");
    QCOMPARE(searcher.search(emptyOpt).size(), 0);

    // Content not present -> no hit.
    SearchOption miss;
    miss.m_keyword = QStringLiteral("zzznotpresent");
    miss.m_objects = SearchObjects(ObjContent);
    QCOMPARE(searcher.search(miss).size(), 0);
  }

  void testIncrementalReindex() {
    QTemporaryDir dir;
    QSharedPointer<Node> note;
    {
      auto *backend = new LocalNotebookBackend(dir.path());
      Notebook nb(1, dir.path(), backend);
      nb.createOnDisk(QStringLiteral("nb"), QString());
      makeNote(nb, nb.getRootNode(), QStringLiteral("doc.md"),
               QStringLiteral("original content only"));
    }

    NotebookMgr mgr;
    auto nb = mgr.openNotebook(dir.path());
    QVERIFY(nb);
    mgr.setCurrentNotebook(nb->getId());
    Searcher searcher(&mgr);

    // Build the index (first content search).
    SearchOption opt;
    opt.m_keyword = QStringLiteral("gamma");
    opt.m_objects = SearchObjects(ObjContent);
    QCOMPARE(searcher.search(opt).size(), 0); // gamma not there yet

    // Rewrite the file to add "gamma" and re-index that node incrementally.
    auto nodes = nb->collectMarkdownNodes();
    QCOMPARE(nodes.size(), 1);
    QFile f(nodes.first()->fetchAbsolutePath());
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    f.write(QByteArray("now with gamma keyword"));
    f.close();
    searcher.reindexNode(nb.data(), nodes.first());

    QCOMPARE(searcher.search(opt).size(), 1); // gamma now indexed
  }
};

QTEST_GUILESS_MAIN(TestSearch)
#include "test_search.moc"
