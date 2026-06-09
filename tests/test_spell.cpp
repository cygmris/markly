#include <QtTest>

#include <core/spell/spellchecker.h>

#ifndef SRC_DICT_DIR
#define SRC_DICT_DIR ""
#endif

using namespace markly;

// Unit tests for the Hunspell-backed SpellChecker (#8c). GUILESS.
class TestSpell : public QObject {
  Q_OBJECT
private slots:
  void checksKnownAndUnknownWords();
  void suggestsForMisspelling();
  void degradesGracefullyWithoutDictionary();
  void ignoreListMakesWordCorrect();
};

void TestSpell::checksKnownAndUnknownWords() {
  SpellChecker sc(QStringLiteral(SRC_DICT_DIR), QStringLiteral("en_US"));
  QVERIFY2(sc.ready(), "en_US dictionary should load from SRC_DICT_DIR");
  QVERIFY(sc.check(QStringLiteral("hello")));
  QVERIFY(!sc.check(QStringLiteral("helllo")));
}

void TestSpell::suggestsForMisspelling() {
  SpellChecker sc(QStringLiteral(SRC_DICT_DIR), QStringLiteral("en_US"));
  QVERIFY(sc.ready());
  const QStringList sugg = sc.suggest(QStringLiteral("helllo"));
  QVERIFY2(!sugg.isEmpty(), "expected suggestions for a misspelled word");
}

void TestSpell::degradesGracefullyWithoutDictionary() {
  SpellChecker sc(QStringLiteral("/no/such/dict/dir"), QStringLiteral("en_US"));
  QVERIFY(!sc.ready());
  // Safe degrade: never flags a word when no dictionary is available.
  QVERIFY(sc.check(QStringLiteral("helllo")));
  QVERIFY(sc.suggest(QStringLiteral("helllo")).isEmpty());
}

void TestSpell::ignoreListMakesWordCorrect() {
  SpellChecker sc(QStringLiteral(SRC_DICT_DIR), QStringLiteral("en_US"));
  QVERIFY(sc.ready());
  QVERIFY(!sc.check(QStringLiteral("helllo"))); // misspelled before ignoring
  sc.addIgnore(QStringLiteral("helllo"));
  QVERIFY(sc.check(QStringLiteral("helllo")));   // ignored -> counts as correct
  QVERIFY(sc.isIgnored(QStringLiteral("HELLLO"))); // case-insensitive
}

QTEST_MAIN(TestSpell)
#include "test_spell.moc"
