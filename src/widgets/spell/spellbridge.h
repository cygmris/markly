#ifndef SPELLBRIDGE_H
#define SPELLBRIDGE_H

#include <QObject>
#include <QStringList>

namespace markly {
// QML bridge (context property "Spell") for the editor's right-click spell menu (#8d).
// Thin delegate over the shared SpellChecker (MarklyApp::getSpellChecker); the session
// ignore list lives in SpellChecker so the underline (#8c) honors it too.
class SpellBridge : public QObject {
  Q_OBJECT
public:
  explicit SpellBridge(QObject *p_parent = nullptr);

  // Spell-check is on and a dictionary is loaded.
  Q_INVOKABLE bool enabled() const;

  // True if the word should be flagged (enabled, non-empty, not ignored, not in dict).
  Q_INVOKABLE bool misspelled(const QString &p_word) const;

  // Up to 8 Hunspell suggestions for the word.
  Q_INVOKABLE QStringList suggest(const QString &p_word) const;

  // Add the word to the session ignore list and notify (so the editor re-highlights).
  Q_INVOKABLE void ignore(const QString &p_word);

signals:
  void ignored();
};
} // namespace markly

#endif // SPELLBRIDGE_H
