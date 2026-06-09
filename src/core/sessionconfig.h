#ifndef SESSIONCONFIG_H
#define SESSIONCONFIG_H

#include "iconfig.h"

#include <QByteArray>
#include <QString>

namespace markly {
// Session configuration (stored in a separate session.json, its own top config).
// NOTE: foundation subset — window geometry/state + default notebook root.
// Later specs (#6 buffers, #13 quick-access/history) extend with opened buffers,
// recent notebooks, view layout, etc.
class SessionConfig : public IConfig {
public:
  explicit SessionConfig(ConfigMgr *p_mgr);

  ~SessionConfig();

  void init() Q_DECL_OVERRIDE;

  void writeToSettings() const Q_DECL_OVERRIDE;

  QJsonObject toJson() const Q_DECL_OVERRIDE;

  QByteArray getMainWindowGeometry() const;
  void setMainWindowGeometry(const QByteArray &p_geometry);

  QByteArray getMainWindowState() const;
  void setMainWindowState(const QByteArray &p_state);

  const QString &getNewNotebookDefaultRootFolderPath() const;
  void setNewNotebookDefaultRootFolderPath(const QString &p_path);

private:
  QByteArray m_mainWindowGeometry;

  QByteArray m_mainWindowState;

  QString m_newNotebookDefaultRootFolderPath;
};
} // namespace markly

#endif // SESSIONCONFIG_H
