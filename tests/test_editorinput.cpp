#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

#include <core/configmgr.h>
#include <core/editor/editinputhelper.h>
#include <core/notebook/node.h>
#include <core/notebook/notebook.h>
#include <core/notebookbackend/localnotebookbackend.h>
#include <core/notebookmgr.h>
#include <core/search/searcher.h>

using namespace markly;

class TestEditorInput : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }

  void testContinueUnordered() {
    auto r = EditInputHelper::continueOnEnter(QStringLiteral("- item"), true, true);
    QVERIFY(!r.clearMarker);
    QCOMPARE(r.insert, QStringLiteral("\n- "));
  }

  void testContinueOrdered() {
    auto r = EditInputHelper::continueOnEnter(QStringLiteral("  3. third"), true, true);
    QCOMPARE(r.insert, QStringLiteral("\n  4. "));
  }

  void testContinueTask() {
    auto r = EditInputHelper::continueOnEnter(QStringLiteral("- [x] done"), true, true);
    QCOMPARE(r.insert, QStringLiteral("\n- [ ] "));
  }

  void testClearEmptyMarker() {
    auto r = EditInputHelper::continueOnEnter(QStringLiteral("  - "), true, true);
    QVERIFY(r.clearMarker);
    QCOMPARE(r.lineIndent, QStringLiteral("  "));
  }

  void testPlainIndentKept() {
    auto r = EditInputHelper::continueOnEnter(QStringLiteral("    code line"), true, true);
    QCOMPARE(r.insert, QStringLiteral("\n    "));
    auto r2 = EditInputHelper::continueOnEnter(QStringLiteral("    code line"), false, true);
    QCOMPARE(r2.insert, QStringLiteral("\n"));
  }

  void testContinueListDisabled() {
    auto r = EditInputHelper::continueOnEnter(QStringLiteral("- item"), true, false);
    QVERIFY(!r.clearMarker);
    QCOMPARE(r.insert, QStringLiteral("\n")); // no indent, no marker
  }

  void testIndentOutdent() {
    QCOMPARE(EditInputHelper::indentLines(QStringLiteral("a\nb"), QStringLiteral("  ")),
             QStringLiteral("  a\n  b"));
    QCOMPARE(EditInputHelper::outdentLines(QStringLiteral("    a\n  b"), QStringLiteral("  ")),
             QStringLiteral("  a\nb"));
    QCOMPARE(EditInputHelper::outdentLines(QStringLiteral("\tx"), QStringLiteral("\t")),
             QStringLiteral("x"));
  }

  void testPairFor() {
    QCOMPARE(EditInputHelper::pairFor(QLatin1Char('(')), QLatin1Char(')'));
    QCOMPARE(EditInputHelper::pairFor(QLatin1Char('"')), QLatin1Char('"'));
    QCOMPARE(EditInputHelper::pairFor(QLatin1Char('a')).unicode(), char16_t(0));
  }

  void testSearchLineNumber() {
    QTemporaryDir dir;
    {
      auto *backend = new LocalNotebookBackend(dir.path());
      Notebook nb(1, dir.path(), backend);
      nb.createOnDisk(QStringLiteral("nb"), QString());
      auto note = nb.newNode(nb.getRootNode(), Node::Type::File, QStringLiteral("doc.md"));
      QFile f(note->fetchAbsolutePath());
      f.open(QIODevice::WriteOnly | QIODevice::Text);
      f.write(QByteArray("line one\nline two\nhere is needle word\nlast\n"));
      f.close();
    }
    NotebookMgr mgr;
    auto nb = mgr.openNotebook(dir.path());
    QVERIFY(nb);
    mgr.setCurrentNotebook(nb->getId());
    Searcher searcher(&mgr);

    SearchOption opt;
    opt.m_keyword = QStringLiteral("needle");
    opt.m_objects = SearchObjects(ObjContent);
    auto res = searcher.search(opt);
    QCOMPARE(res.size(), 1);
    QCOMPARE(res.first().m_line, 3); // "needle" is on line 3 (1-based)
  }
};

QTEST_GUILESS_MAIN(TestEditorInput)
#include "test_editorinput.moc"
