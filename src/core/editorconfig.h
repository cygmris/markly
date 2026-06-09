#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H

#include "iconfig.h"

namespace markly {
// Editor configuration. Concrete fields added by the markdown-editor specs;
// unknown keys are round-tripped to avoid data loss.
class EditorConfig : public IConfig {
public:
  EditorConfig(ConfigMgr *p_mgr, IConfig *p_topConfig);

  void init(const QJsonObject &p_app, const QJsonObject &p_user) Q_DECL_OVERRIDE;

  QJsonObject toJson() const Q_DECL_OVERRIDE;

  int getFontSize() const { return m_fontSize; }
  void setFontSize(int p_size);

  int getTabWidth() const { return m_tabWidth; }
  void setTabWidth(int p_width);

  bool isExpandTab() const { return m_expandTab; }
  void setExpandTab(bool p_on);

  bool isLineNumberEnabled() const { return m_lineNumber; }
  void setLineNumberEnabled(bool p_on);

  bool isHighlightCurrentLineEnabled() const { return m_highlightCurrentLine; }
  void setHighlightCurrentLineEnabled(bool p_on);

private:
  QJsonObject m_jobj;

  int m_fontSize = 14;
  int m_tabWidth = 4;
  bool m_expandTab = true;
  bool m_lineNumber = true;
  bool m_highlightCurrentLine = true;
};
} // namespace markly

#endif // EDITORCONFIG_H
