#include "editorconfig.h"

using namespace markly;

EditorConfig::EditorConfig(ConfigMgr *p_mgr, IConfig *p_topConfig)
    : IConfig(p_mgr, p_topConfig) {}

void EditorConfig::init(const QJsonObject &p_app, const QJsonObject &p_user) {
  m_sessionName = QStringLiteral("editor");

  // Merge: app defaults overridden by user keys (preserves unknown keys).
  m_jobj = p_app;
  for (auto it = p_user.begin(); it != p_user.end(); ++it) {
    m_jobj[it.key()] = it.value();
  }

  auto readInt = [this](const QString &k, int def) {
    return m_jobj.contains(k) ? m_jobj.value(k).toInt(def) : def;
  };
  auto readBool = [this](const QString &k, bool def) {
    return m_jobj.contains(k) ? m_jobj.value(k).toBool(def) : def;
  };
  m_fontSize = readInt(QStringLiteral("font_size"), 14);
  m_tabWidth = readInt(QStringLiteral("tab_width"), 4);
  m_expandTab = readBool(QStringLiteral("expand_tab"), true);
  m_lineNumber = readBool(QStringLiteral("line_number"), true);
  m_highlightCurrentLine = readBool(QStringLiteral("highlight_current_line"), true);
  m_autoIndent = readBool(QStringLiteral("auto_indent"), true);
  m_continueList = readBool(QStringLiteral("continue_list"), true);
  m_autoPair = readBool(QStringLiteral("auto_pair"), true);
}

QJsonObject EditorConfig::toJson() const {
  QJsonObject obj = m_jobj;
  obj[QStringLiteral("font_size")] = m_fontSize;
  obj[QStringLiteral("tab_width")] = m_tabWidth;
  obj[QStringLiteral("expand_tab")] = m_expandTab;
  obj[QStringLiteral("line_number")] = m_lineNumber;
  obj[QStringLiteral("highlight_current_line")] = m_highlightCurrentLine;
  obj[QStringLiteral("auto_indent")] = m_autoIndent;
  obj[QStringLiteral("continue_list")] = m_continueList;
  obj[QStringLiteral("auto_pair")] = m_autoPair;
  return obj;
}

void EditorConfig::setFontSize(int p_size) { updateConfig(m_fontSize, p_size, this); }
void EditorConfig::setTabWidth(int p_width) { updateConfig(m_tabWidth, p_width, this); }
void EditorConfig::setExpandTab(bool p_on) { updateConfig(m_expandTab, p_on, this); }
void EditorConfig::setLineNumberEnabled(bool p_on) { updateConfig(m_lineNumber, p_on, this); }
void EditorConfig::setHighlightCurrentLineEnabled(bool p_on) {
  updateConfig(m_highlightCurrentLine, p_on, this);
}
void EditorConfig::setAutoIndentEnabled(bool p_on) { updateConfig(m_autoIndent, p_on, this); }
void EditorConfig::setContinueListEnabled(bool p_on) { updateConfig(m_continueList, p_on, this); }
void EditorConfig::setAutoPairEnabled(bool p_on) { updateConfig(m_autoPair, p_on, this); }
