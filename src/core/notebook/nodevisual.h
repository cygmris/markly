#ifndef NODEVISUAL_H
#define NODEVISUAL_H

#include <QString>

namespace markly {
// Per-node visual customization: background / border / name color.
// Empty string means "no override / inherit". Ported from VNote NodeVisual.
class NodeVisual {
public:
  NodeVisual() = default;

  NodeVisual(const QString &p_backgroundColor, const QString &p_borderColor,
             const QString &p_nameColor)
      : m_backgroundColor(p_backgroundColor), m_borderColor(p_borderColor),
        m_nameColor(p_nameColor) {}

  const QString &getBackgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(const QString &p_color) { m_backgroundColor = p_color; }

  const QString &getBorderColor() const { return m_borderColor; }
  void setBorderColor(const QString &p_color) { m_borderColor = p_color; }

  const QString &getNameColor() const { return m_nameColor; }
  void setNameColor(const QString &p_color) { m_nameColor = p_color; }

  bool hasAnyVisualEffect() const {
    return !m_backgroundColor.isEmpty() || !m_borderColor.isEmpty() || !m_nameColor.isEmpty();
  }

  void clearAllColors() {
    m_backgroundColor.clear();
    m_borderColor.clear();
    m_nameColor.clear();
  }

private:
  QString m_backgroundColor;
  QString m_borderColor;
  QString m_nameColor;
};
} // namespace markly

#endif // NODEVISUAL_H
