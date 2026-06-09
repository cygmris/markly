#ifndef MARKLY_HISTORYMGR_H
#define MARKLY_HISTORYMGR_H

#include <QObject>
#include <QStringList>

namespace markly {
// Tracks recently opened notes (MRU history), user-pinned notes (quick access), and
// the flash scratch note. Persists history/quick-access via SessionConfig. The flash
// note lives at <AppConfigLocation>/flash.md. Source in markly_core for unit testing.
class HistoryMgr : public QObject {
  Q_OBJECT
public:
  explicit HistoryMgr(QObject *p_parent = nullptr);

  void load();

  void addToHistory(const QString &p_path);
  QStringList history() const { return m_history; }

  void pin(const QString &p_path);
  void unpin(const QString &p_path);
  bool isPinned(const QString &p_path) const { return m_quickAccess.contains(p_path); }
  QStringList quickAccess() const { return m_quickAccess; }

  // Path to the flash scratch note; creates it (with a template) if absent.
  QString ensureFlashFile() const;

  static const int c_maxHistory;

signals:
  void changed();

private:
  void persist();

  QStringList m_history;     // MRU, most recent first
  QStringList m_quickAccess; // pinned
};
} // namespace markly

#endif // MARKLY_HISTORYMGR_H
