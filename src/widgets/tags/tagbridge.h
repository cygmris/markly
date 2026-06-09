#ifndef MARKLY_TAGBRIDGE_H
#define MARKLY_TAGBRIDGE_H

#include <QObject>
#include <QString>
#include <QVariantList>

namespace markly {
class NotebookMgr;

// Read-only bridge driving the tags panel (context property "Tags"). Lists the
// current notebook's tags with note counts, and the notes carrying a selected tag.
// Refreshes on notebook switch and on MarklyApp::tagsChanged. Source lives in
// markly_core so it can be unit-tested.
class TagBridge : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList tags READ tags NOTIFY changed)
  Q_PROPERTY(QString selectedTag READ selectedTag NOTIFY changed)
  Q_PROPERTY(QVariantList taggedNodes READ taggedNodes NOTIFY changed)
public:
  explicit TagBridge(NotebookMgr *p_mgr, QObject *p_parent = nullptr);

  QVariantList tags() const { return m_tags; }
  QString selectedTag() const { return m_selectedTag; }
  QVariantList taggedNodes() const { return m_taggedNodes; }

  Q_INVOKABLE void selectTag(const QString &p_name);
  Q_INVOKABLE void openTaggedNode(int p_index);
  Q_INVOKABLE void refresh();

signals:
  void changed();

private:
  void rebuildTaggedNodes();

  NotebookMgr *m_mgr = nullptr;
  QVariantList m_tags;        // [{name, count}]
  QString m_selectedTag;
  QVariantList m_taggedNodes; // [{name, path}]
};
} // namespace markly

#endif // MARKLY_TAGBRIDGE_H
