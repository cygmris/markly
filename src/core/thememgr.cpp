#include "thememgr.h"

#include "theme/appearance.h"
#include "theme/stylesheetgenerator.h"
#include "theme/thememodel.h"

using namespace markly;

ThemeMgr::ThemeMgr(QObject *p_parent) : QObject(p_parent) {
  m_appearance = new Appearance(this);

  connect(m_appearance, &Appearance::changed, this, &ThemeMgr::refreshCurrentTheme);

  refreshCurrentTheme();
}

void ThemeMgr::refreshCurrentTheme() {
  m_tokens = ThemeModel::buildTokens(m_appearance->getStyle(), m_appearance->resolvedDark(),
                                     m_appearance->accentOverride());
  m_tokens.contentZoom = m_appearance->contentZoom();
  emit themeChanged();
}

QString ThemeMgr::fetchQtStyleSheet() const { return StyleSheetGenerator::generate(m_tokens); }
