#ifndef MARKLY_GLOBALHOTKEY_H
#define MARKLY_GLOBALHOTKEY_H

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <QString>

namespace markly {
// OS-global hotkey to summon the window (#17b). Registers via X11/XCB only when the
// Qt platform is "xcb"; on Wayland/offscreen it is a safe no-op (the single-instance
// raise is the portable fallback). Emits activated() on the key combination.
class GlobalHotkey : public QObject, public QAbstractNativeEventFilter {
  Q_OBJECT
public:
  explicit GlobalHotkey(QObject *p_parent = nullptr);
  ~GlobalHotkey() override;

  // Returns true if the hotkey was registered (only possible on xcb + valid sequence).
  bool registerHotkey(const QString &p_seq);
  void unregister();

  bool nativeEventFilter(const QByteArray &p_eventType, void *p_message,
                         qintptr *p_result) override;

signals:
  void activated();

private:
  bool m_registered = false;
  quint32 m_keycode = 0;
  quint32 m_modifiers = 0;
};
} // namespace markly

#endif // MARKLY_GLOBALHOTKEY_H
