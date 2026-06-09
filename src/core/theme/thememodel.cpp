#include "thememodel.h"

#include <utils/colorutils.h>

using namespace markly;

namespace {
inline QColor hex(const char *p_hex) { return QColor(QString::fromLatin1(p_hex)); }
} // namespace

QColor ThemeModel::accentTextFor(const QColor &p_accent) {
  return ColorUtils::contrastingText(p_accent);
}

QColor ThemeModel::defaultAccent(StyleId p_style, bool p_dark) {
  switch (p_style) {
  case StyleId::Refined:
    return p_dark ? hex("#2fbf9b") : hex("#0e8c6f");
  case StyleId::Focus:
    return p_dark ? hex("#e0905c") : hex("#bd6234");
  case StyleId::Workbench:
    return p_dark ? hex("#8b6cf0") : hex("#6c4be0");
  }
  return hex("#0e8c6f");
}

ThemeTokens ThemeModel::buildTokens(StyleId p_style, bool p_dark, const QColor &p_accentOverride) {
  QColor accent = p_accentOverride.isValid() ? p_accentOverride : defaultAccent(p_style, p_dark);
  ThemeTokens t;
  switch (p_style) {
  case StyleId::Refined:
    t = buildRefinedA(p_dark, accent);
    break;
  case StyleId::Focus:
    t = buildFocusB(p_dark, accent);
    break;
  case StyleId::Workbench:
    t = buildWorkbenchC(p_dark, accent);
    break;
  }

  t.fontUi = themedefs::fontUi();
  t.fontSerif = themedefs::fontSerif();
  t.fontDisplay = themedefs::fontDisplay();
  t.fontMono = themedefs::fontMono();
  t.isDark = p_dark;
  return t;
}

// ---- Direction A: Refined Classic / Graphite Teal (refined.jsx tok) ----
ThemeTokens ThemeModel::buildRefinedA(bool p_dark, const QColor &p_acc) {
  ThemeTokens t;
  const QColor acc = p_acc;
  if (p_dark) {
    t.window = hex("#14171a");
    t.rail = hex("#101315");
    t.sidebar = hex("#181c1f");
    t.canvas = hex("#16191c");
    t.card = hex("#16191c");
    t.bar = hex("#141719");
    t.border = ColorUtils::rgba(255, 255, 255, 0.08);
    t.borderStrong = ColorUtils::rgba(255, 255, 255, 0.13);
    t.text = hex("#e7ebee");
    t.dim = hex("#99a3ab");
    t.faint = hex("#67717a");
    t.gutter = hex("#525c64");
    t.accent = acc;
    t.accentText = hex("#0c1311");
    t.selection = ColorUtils::alpha(acc, 0.16);
    t.accentSoft = ColorUtils::alpha(acc, 0.18); // mark
    t.hover = ColorUtils::rgba(255, 255, 255, 0.06);
    t.heading = acc;
    t.emphasis = hex("#e0915f");
    t.codeInline = hex("#cf8be0");
    t.link = hex("#5aa9e6");
    t.codeBg = hex("#101315");
    t.codeInk = hex("#e7ebee");
    t.windowButton = hex("#7a838b");
  } else {
    t.window = hex("#e8ebed");
    t.rail = hex("#e9ecee");
    t.sidebar = hex("#f5f6f7");
    t.canvas = hex("#ffffff");
    t.card = hex("#ffffff");
    t.bar = hex("#eef0f2");
    t.border = ColorUtils::rgba(15, 25, 35, 0.09);
    t.borderStrong = ColorUtils::rgba(15, 25, 35, 0.14);
    t.text = hex("#1d2528");
    t.dim = hex("#5a6670");
    t.faint = hex("#8b949c");
    t.gutter = hex("#aab2b9");
    t.accent = acc;
    t.accentText = hex("#ffffff");
    t.selection = ColorUtils::alpha(acc, 0.12);
    t.accentSoft = ColorUtils::alpha(acc, 0.14); // mark
    t.hover = ColorUtils::rgba(15, 25, 35, 0.05);
    t.heading = acc;
    t.emphasis = hex("#c2683c");
    t.codeInline = hex("#9a52c4");
    t.link = hex("#2f72c4");
    t.codeBg = hex("#f5f6f7");
    t.codeInk = hex("#1d2528");
    t.windowButton = hex("#7a838b");
  }
  // A has no dedicated rail-text / extra colors: fall back to general roles.
  t.railText = t.dim;
  t.railDim = t.faint;
  t.railActive = t.text;
  t.lime = t.accent;
  t.amber = t.emphasis;
  t.pink = t.codeInline;
  t.sage = t.dim;
  return t;
}

// ---- Direction B: Focus Writing / Warm Paper (focus.jsx tok) ----
ThemeTokens ThemeModel::buildFocusB(bool p_dark, const QColor &p_acc) {
  ThemeTokens t;
  const QColor acc = p_acc;
  if (p_dark) {
    t.window = hex("#1c1813");
    t.rail = hex("#191510");
    t.sidebar = hex("#221d17"); // list
    t.canvas = hex("#201b15");  // paper
    t.card = hex("#201b15");
    t.bar = hex("#1c1813");
    t.border = ColorUtils::rgba(255, 240, 220, 0.09);
    t.borderStrong = ColorUtils::rgba(255, 240, 220, 0.14);
    t.text = hex("#ece3d5"); // ink
    t.dim = hex("#a89a87");
    t.faint = hex("#736857");
    t.accent = acc;
    t.sage = hex("#9fb088");
    t.selection = ColorUtils::alpha(acc, 0.15);
    t.accentSoft = ColorUtils::alpha(acc, 0.08); // quoteBg
    t.hover = ColorUtils::rgba(255, 240, 220, 0.05);
    t.codeBg = hex("#191510");
    t.codeInk = hex("#d9cdb8");
    t.windowButton = hex("#8a7d6b");
  } else {
    t.window = hex("#f1ebe0");
    t.rail = hex("#ebe3d4");
    t.sidebar = hex("#f0e9db"); // list
    t.canvas = hex("#fbf8f1");  // paper
    t.card = hex("#fbf8f1");
    t.bar = hex("#f1ebe0");
    t.border = ColorUtils::rgba(70, 52, 28, 0.12);
    t.borderStrong = ColorUtils::rgba(70, 52, 28, 0.18);
    t.text = hex("#2e2a23"); // ink
    t.dim = hex("#6b6052");
    t.faint = hex("#9a8f7e");
    t.accent = acc;
    t.sage = hex("#6f8160");
    t.selection = ColorUtils::alpha(acc, 0.12);
    t.accentSoft = ColorUtils::alpha(acc, 0.07); // quoteBg
    t.hover = ColorUtils::rgba(70, 52, 28, 0.05);
    t.codeBg = hex("#f2ece1");
    t.codeInk = hex("#4a4236");
    t.windowButton = hex("#9a8f7e");
  }
  t.gutter = t.faint;
  t.accentText = accentTextFor(acc);
  t.heading = t.text; // B headings are serif ink
  t.emphasis = acc;
  t.codeInline = acc; // accentDeep == accent
  t.link = acc;
  t.railText = t.dim;
  t.railDim = t.faint;
  t.railActive = t.text;
  t.lime = t.sage;
  t.amber = t.emphasis;
  t.pink = t.accent;
  return t;
}

// ---- Direction C: Bold Workbench / Ink & Electric (bold.jsx tok) ----
ThemeTokens ThemeModel::buildWorkbenchC(bool p_dark, const QColor &p_acc) {
  ThemeTokens t;
  const QColor acc = p_acc;
  if (p_dark) {
    t.window = hex("#101116");
    t.rail = hex("#15161d"); // nav
    t.sidebar = hex("#15161d");
    t.canvas = hex("#131419");
    t.card = hex("#1b1d25");
    t.bar = hex("#15161d");
    t.border = ColorUtils::rgba(255, 255, 255, 0.08);
    t.borderStrong = ColorUtils::rgba(255, 255, 255, 0.13);
    t.text = hex("#e7e9ef");
    t.dim = hex("#969cab");
    t.faint = hex("#646a78");
    t.railText = hex("#cdd1dc");
    t.railDim = hex("#7d8493");
    t.railActive = hex("#ffffff");
    t.accent = acc;
    t.accentText = hex("#ffffff");
    t.accentSoft = ColorUtils::alpha(acc, 0.16);
    t.selection = ColorUtils::alpha(acc, 0.20); // navSel
    t.lime = hex("#b6e04b");
    t.amber = hex("#e6a23c");
    t.pink = hex("#ee6aa0");
    t.hover = ColorUtils::rgba(255, 255, 255, 0.05);
    t.codeBg = hex("#0e0f14");
    t.codeInk = hex("#cfd3de");
    t.windowButton = hex("#7d8493");
  } else {
    t.window = hex("#eceef2");
    t.rail = hex("#1a1c24"); // nav (dark even in light mode)
    t.sidebar = hex("#1a1c24");
    t.canvas = hex("#f6f7f9");
    t.card = hex("#ffffff");
    t.bar = hex("#ffffff");
    t.border = ColorUtils::rgba(18, 20, 28, 0.09);
    t.borderStrong = ColorUtils::rgba(18, 20, 28, 0.14);
    t.text = hex("#1a1c26");
    t.dim = hex("#5b6273");
    t.faint = hex("#9197a5");
    t.railText = hex("#c8ccd8");
    t.railDim = hex("#7a8191");
    t.railActive = hex("#ffffff");
    t.accent = acc;
    t.accentText = hex("#ffffff");
    t.accentSoft = ColorUtils::alpha(acc, 0.10);
    t.selection = ColorUtils::alpha(acc, 0.24); // navSel
    t.lime = hex("#5f8f15");
    t.amber = hex("#c07d1c");
    t.pink = hex("#d4488a");
    t.hover = ColorUtils::rgba(18, 20, 28, 0.05);
    t.codeBg = hex("#1a1c24");
    t.codeInk = hex("#d6d9e2");
    t.windowButton = hex("#888f9e");
  }
  t.gutter = t.faint;
  t.heading = t.text;
  t.emphasis = acc;
  t.codeInline = acc;
  t.link = acc;
  t.sage = t.lime;
  return t;
}
