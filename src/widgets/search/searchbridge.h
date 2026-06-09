#ifndef MARKLY_SEARCHBRIDGE_H
#define MARKLY_SEARCHBRIDGE_H

#include <QObject>
#include <QScopedPointer>
#include <QVariantList>
#include <QVector>

#include <core/search/searchdata.h>

namespace markly {
class NotebookMgr;
class Searcher;

// Bridge between the full-text Searcher (#12) and the QML shells. Injected into QML
// as context property "Search". Exposes results as a QML-bindable list and opens a
// result via MarklyApp::openFileRequested. Listens to MarklyApp::noteSaved to keep
// the FTS index incrementally fresh.
class SearchBridge : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList results READ results NOTIFY resultsChanged)
  Q_PROPERTY(int count READ count NOTIFY resultsChanged)
  Q_PROPERTY(QString state READ state NOTIFY stateChanged)
  Q_PROPERTY(QString keyword READ keyword NOTIFY stateChanged)
public:
  explicit SearchBridge(NotebookMgr *p_notebookMgr, QObject *p_parent = nullptr);
  ~SearchBridge() override;

  QVariantList results() const { return m_results; }
  int count() const { return m_items.size(); }
  QString state() const { return m_state; }
  QString keyword() const { return m_keyword; }

  // scope: SearchScope as int; objects: SearchObjects bitmask as int.
  Q_INVOKABLE void search(const QString &p_keyword, int p_scope, int p_objects,
                          const QString &p_folderRelPath);
  Q_INVOKABLE void openResult(int p_index);
  Q_INVOKABLE void clear();

signals:
  void resultsChanged();
  void stateChanged();

private slots:
  void onNoteSaved(const QString &p_filePath);

private:
  void setState(const QString &p_state);

  NotebookMgr *m_notebookMgr = nullptr;
  QScopedPointer<Searcher> m_searcher;
  QVector<SearchResultItem> m_items;
  QVariantList m_results;
  QString m_state = QStringLiteral("idle");
  QString m_keyword;
};
} // namespace markly

#endif // MARKLY_SEARCHBRIDGE_H
