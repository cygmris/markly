#ifndef MARKLY_SNIPPETBRIDGE_H
#define MARKLY_SNIPPETBRIDGE_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

namespace markly {
class SnippetMgr;

// Bridge for snippets (context property "Snippets"). Source in markly_core.
class SnippetBridge : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList list READ list NOTIFY changed)
public:
  explicit SnippetBridge(SnippetMgr *p_mgr, QObject *p_parent = nullptr);

  QVariantList list() const; // [{name, description}]

  // Returns { text, cursorOffset }.
  Q_INVOKABLE QVariantMap apply(const QString &p_name, const QString &p_selectedText,
                                const QString &p_noteName);
  Q_INVOKABLE void add(const QString &p_name, const QString &p_content);
  Q_INVOKABLE void remove(const QString &p_name);

signals:
  void changed();

private:
  SnippetMgr *m_mgr = nullptr;
};
} // namespace markly

#endif // MARKLY_SNIPPETBRIDGE_H
