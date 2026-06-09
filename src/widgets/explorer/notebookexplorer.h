#ifndef NOTEBOOKEXPLORER_H
#define NOTEBOOKEXPLORER_H

#include <QHash>
#include <QObject>
#include <QSet>
#include <QSharedPointer>
#include <QVariantList>
#include <QWeakPointer>

#include <core/global.h>

namespace markly {
class NotebookMgr;
class Notebook;
class Node;

// Bridge between the notebook data layer (#4) and the QML shells (#3).
// Flattens the current notebook's Node tree into a QML-bindable visibleNodes list
// (respecting expand state, selection, per-node colors, external files) and exposes
// Q_INVOKABLE interactions that write through the Notebook facade (which keeps disk
// + sqlite consistent). Injected into QML as context property "Explorer".
class NotebookExplorer : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList visibleNodes READ visibleNodes NOTIFY changed)
  Q_PROPERTY(QVariantList notebooks READ notebooks NOTIFY changed)
  Q_PROPERTY(QString currentNotebookName READ currentNotebookName NOTIFY changed)
  Q_PROPERTY(bool hasNotebook READ hasNotebook NOTIFY changed)
  Q_PROPERTY(double selectedNodeId READ selectedNodeId NOTIFY changed)
public:
  explicit NotebookExplorer(NotebookMgr *p_mgr, QObject *p_parent = nullptr);

  QVariantList visibleNodes() const { return m_visibleNodes; }
  QVariantList notebooks() const;
  QString currentNotebookName() const;
  bool hasNotebook() const;
  double selectedNodeId() const { return static_cast<double>(m_selectedId); }

  // --- interactions (return "" on success, error message otherwise where noted) ---
  Q_INVOKABLE void toggleExpand(double p_nodeId);
  Q_INVOKABLE void selectNode(double p_nodeId);
  Q_INVOKABLE QString newNote(double p_parentId, const QString &p_name);
  Q_INVOKABLE QString newFolder(double p_parentId, const QString &p_name);
  Q_INVOKABLE QString renameNode(double p_nodeId, const QString &p_name);
  Q_INVOKABLE void removeToRecycle(double p_nodeId);
  Q_INVOKABLE void removePermanent(double p_nodeId);
  Q_INVOKABLE void setNodeColor(double p_nodeId, const QString &p_bg, const QString &p_border,
                                const QString &p_name);
  Q_INVOKABLE void clearNodeColor(double p_nodeId);
  Q_INVOKABLE void importExternal(double p_parentId, const QString &p_name);
  Q_INVOKABLE void switchNotebook(double p_notebookId);
  Q_INVOKABLE bool newNotebookAt(const QString &p_rootDir, const QString &p_name,
                                 const QString &p_description);
  Q_INVOKABLE bool openNotebookAt(const QString &p_rootDir);

signals:
  void changed();

private slots:
  void onCurrentNotebookChanged();

private:
  void rebuild();
  void appendChildren(const QSharedPointer<Node> &p_folder, int p_depth);
  void appendExternal(const QSharedPointer<Node> &p_folder, int p_depth);
  QSharedPointer<Node> resolveNode(ID p_id) const;
  QSharedPointer<Notebook> currentNotebook() const;
  QString validateName(const QSharedPointer<Node> &p_parent, const QString &p_name) const;
  static bool isIgnoredEntry(const QString &p_name);

  NotebookMgr *m_mgr = nullptr;
  QSet<ID> m_expanded;
  ID m_selectedId = 0;

  QVariantList m_visibleNodes;
  QHash<ID, QWeakPointer<Node>> m_nodeMap;
};
} // namespace markly

#endif // NOTEBOOKEXPLORER_H
