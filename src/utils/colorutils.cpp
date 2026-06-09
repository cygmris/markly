#include "colorutils.h"

#include <algorithm>

using namespace markly;

QColor ColorUtils::alpha(const QColor &p_base, qreal p_alpha) {
  QColor c(p_base);
  c.setAlphaF(std::clamp(p_alpha, 0.0, 1.0));
  return c;
}

QColor ColorUtils::rgb(int p_r, int p_g, int p_b) { return QColor(p_r, p_g, p_b); }

QColor ColorUtils::rgba(int p_r, int p_g, int p_b, qreal p_alpha) {
  QColor c(p_r, p_g, p_b);
  c.setAlphaF(std::clamp(p_alpha, 0.0, 1.0));
  return c;
}

qreal ColorUtils::luminance(const QColor &p_color) {
  // Simple perceptual luminance on straight (non-linear) sRGB channels.
  return (0.299 * p_color.redF() + 0.587 * p_color.greenF() + 0.114 * p_color.blueF());
}

QColor ColorUtils::contrastingText(const QColor &p_background) {
  return luminance(p_background) > 0.55 ? QColor(QStringLiteral("#000000"))
                                       : QColor(QStringLiteral("#ffffff"));
}
