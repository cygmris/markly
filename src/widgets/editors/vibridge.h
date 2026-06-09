#ifndef MARKLY_VIBRIDGE_H
#define MARKLY_VIBRIDGE_H

#include <QObject>
#include <QVariantMap>

#include <core/editor/viengine.h>

namespace markly {
// QML bridge (context property "Vi") over ViEngine (#8b). The editor calls handleKey
// in Normal/Visual mode and applies the returned edits/cursor/selection/mode.
class ViBridge : public QObject {
  Q_OBJECT
  Q_PROPERTY(int mode READ mode NOTIFY modeChanged) // 0 Normal / 1 Insert / 2 Visual
public:
  explicit ViBridge(QObject *p_parent = nullptr);

  int mode() const { return static_cast<int>(m_engine.mode()); }

  // Vi input is on (EditorConfig.viMode).
  Q_INVOKABLE bool enabled() const;

  // Returns { handled, mode, cursor, anchor, edits: [{start, end, text}, ...] }.
  Q_INVOKABLE QVariantMap handleKey(const QString &p_text, int p_cursor, int p_selStart,
                                    int p_selEnd, const QString &p_keyText, int p_key,
                                    int p_mods);

  Q_INVOKABLE void reset();

signals:
  void modeChanged();

private:
  ViEngine m_engine;
};
} // namespace markly

#endif // MARKLY_VIBRIDGE_H
