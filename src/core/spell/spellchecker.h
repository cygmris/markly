#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QSet>
#include <QString>
#include <QStringList>

namespace markly {
// Thin wrapper over libhunspell (C API) for English spell-checking. Loads
// <dictDir>/<lang>.aff + <dictDir>/<lang>.dic. If the dictionary fails to load,
// ready() is false and check() returns true (safe degrade: never flag words).
class SpellChecker {
public:
  SpellChecker(const QString &p_dictDir, const QString &p_lang);
  ~SpellChecker();

  bool ready() const { return m_handle != nullptr; }

  // True if the word is spelled correctly (or the checker is not ready).
  bool check(const QString &p_word) const;

  // Suggestions for a misspelled word (empty if not ready).
  QStringList suggest(const QString &p_word) const;

  // Session ignore list (case-insensitive): ignored words count as correct (#8d).
  void addIgnore(const QString &p_word);
  bool isIgnored(const QString &p_word) const;

private:
  void *m_handle = nullptr; // Hunhandle* (kept void* to avoid leaking the C header)
  QByteArray m_encoding;    // dictionary encoding (e.g. UTF-8 / ISO8859-1)
  QSet<QString> m_ignore;   // lower-cased ignored words
};
} // namespace markly

#endif // SPELLCHECKER_H
