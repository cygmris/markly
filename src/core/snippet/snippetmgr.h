#ifndef MARKLY_SNIPPETMGR_H
#define MARKLY_SNIPPETMGR_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QString>

namespace markly {
// A reusable text snippet: content with a cursor mark (@@) and selection mark ($$),
// plus magic words (%date% etc) expanded on apply.
struct Snippet {
  QString m_name;
  QString m_description;
  QString m_content;
  QString m_cursorMark = QStringLiteral("@@");
  QString m_selectionMark = QStringLiteral("$$");
  bool isValid() const { return !m_name.isEmpty(); }
};

// Manages snippets stored as JSON under <AppConfigLocation>/snippets/. Source in
// markly_core for unit testing. Also used for "new note from template" (#14).
class SnippetMgr : public QObject {
  Q_OBJECT
public:
  explicit SnippetMgr(QObject *p_parent = nullptr);

  void load();
  QList<Snippet> snippets() const;
  Snippet get(const QString &p_name) const { return m_snippets.value(p_name); }
  void add(const QString &p_name, const QString &p_content);
  void remove(const QString &p_name);

  // Expand the named snippet: magic words, $$ -> selectedText, @@ -> cursor (removed,
  // its index written to p_cursorOffset; end-of-text if absent). "" if not found.
  QString apply(const QString &p_name, const QString &p_selectedText,
                const QString &p_noteName, int &p_cursorOffset) const;

  static QString expandMagicWords(const QString &p_text, const QString &p_noteName);

signals:
  void changed();

private:
  void seedDefaults();
  QString dir() const;
  QString filePath(const QString &p_name) const;
  void writeFile(const Snippet &p_snippet) const;

  QHash<QString, Snippet> m_snippets;
};
} // namespace markly

#endif // MARKLY_SNIPPETMGR_H
