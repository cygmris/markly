#ifndef COLORUTILS_H
#define COLORUTILS_H

#include <QColor>

namespace markly {
// Color helpers for the theme system.
class ColorUtils {
public:
  ColorUtils() = delete;

  // Return @p_base with its alpha replaced by @p_alpha (0..1).
  // Counterpart of the design's vAlpha(hex, a).
  static QColor alpha(const QColor &p_base, qreal p_alpha);

  // Build an opaque QColor from 8-bit channels (convenience for rgb()).
  static QColor rgb(int p_r, int p_g, int p_b);

  // Build a QColor from 8-bit channels + alpha (0..1), like CSS rgba().
  static QColor rgba(int p_r, int p_g, int p_b, qreal p_alpha);

  // Perceived luminance (0..1) using the sRGB-ish weights.
  static qreal luminance(const QColor &p_color);

  // Choose black or white text for best contrast over @p_background.
  static QColor contrastingText(const QColor &p_background);
};
} // namespace markly

#endif // COLORUTILS_H
