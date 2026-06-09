#ifndef MARKLY_TRAYCFG_H
#define MARKLY_TRAYCFG_H

#include <QObject>
#include <QString>

namespace markly {
// QML bridge (context property "TrayCfg") for the minimize-to-tray setting (#20b).
class TrayCfgQml : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool minimizeToTray READ minimizeToTray WRITE setMinimizeToTray NOTIFY changed)
  Q_PROPERTY(bool autoUpdateCheck READ autoUpdateCheck WRITE setAutoUpdateCheck NOTIFY changed)
  Q_PROPERTY(QString globalHotkey READ globalHotkey WRITE setGlobalHotkey NOTIFY changed)
public:
  explicit TrayCfgQml(QObject *p_parent = nullptr);

  bool minimizeToTray() const;
  void setMinimizeToTray(bool p_v);

  bool autoUpdateCheck() const;
  void setAutoUpdateCheck(bool p_v);

  QString globalHotkey() const;
  void setGlobalHotkey(const QString &p_v);

signals:
  void changed();
};
} // namespace markly

#endif // MARKLY_TRAYCFG_H
