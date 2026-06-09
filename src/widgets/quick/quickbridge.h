#ifndef MARKLY_QUICKBRIDGE_H
#define MARKLY_QUICKBRIDGE_H

#include <QObject>
#include <QString>
#include <QVariantList>

namespace markly {
class HistoryMgr;

// Bridge for quick access (pinned), history (recent), and flash (scratch note).
// Injected into QML as context property "Quick". Source in markly_core for testing.
class QuickBridge : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList history READ history NOTIFY changed)
  Q_PROPERTY(QVariantList quickAccess READ quickAccess NOTIFY changed)
public:
  explicit QuickBridge(HistoryMgr *p_mgr, QObject *p_parent = nullptr);

  QVariantList history() const;     // [{name, path}], existing files only
  QVariantList quickAccess() const; // [{name, path}]

  Q_INVOKABLE void open(const QString &p_path);
  Q_INVOKABLE void pin(const QString &p_path);
  Q_INVOKABLE void unpin(const QString &p_path);
  Q_INVOKABLE bool isPinned(const QString &p_path) const;
  Q_INVOKABLE void openFlash();

signals:
  void changed();

private:
  HistoryMgr *m_mgr = nullptr;
};
} // namespace markly

#endif // MARKLY_QUICKBRIDGE_H
