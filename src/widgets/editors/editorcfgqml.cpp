#include "editorcfgqml.h"

#include <core/configmgr.h>
#include <core/editorconfig.h>

using namespace markly;

EditorCfgQml::EditorCfgQml(QObject *p_parent) : QObject(p_parent) {}

static EditorConfig &cfg() { return ConfigMgr::getInst().getEditorConfig(); }

int EditorCfgQml::fontSize() const { return cfg().getFontSize(); }
int EditorCfgQml::tabWidth() const { return cfg().getTabWidth(); }
bool EditorCfgQml::expandTab() const { return cfg().isExpandTab(); }
bool EditorCfgQml::lineNumber() const { return cfg().isLineNumberEnabled(); }
bool EditorCfgQml::highlightCurrentLine() const { return cfg().isHighlightCurrentLineEnabled(); }
bool EditorCfgQml::autoIndent() const { return cfg().isAutoIndentEnabled(); }
bool EditorCfgQml::continueList() const { return cfg().isContinueListEnabled(); }
bool EditorCfgQml::autoPair() const { return cfg().isAutoPairEnabled(); }

void EditorCfgQml::setFontSize(int p_v) {
  cfg().setFontSize(qBound(8, p_v, 32));
  emit changed();
}
void EditorCfgQml::setTabWidth(int p_v) {
  cfg().setTabWidth(qBound(1, p_v, 8));
  emit changed();
}
void EditorCfgQml::setExpandTab(bool p_v) { cfg().setExpandTab(p_v); emit changed(); }
void EditorCfgQml::setLineNumber(bool p_v) { cfg().setLineNumberEnabled(p_v); emit changed(); }
void EditorCfgQml::setHighlightCurrentLine(bool p_v) {
  cfg().setHighlightCurrentLineEnabled(p_v);
  emit changed();
}
void EditorCfgQml::setAutoIndent(bool p_v) { cfg().setAutoIndentEnabled(p_v); emit changed(); }
void EditorCfgQml::setContinueList(bool p_v) { cfg().setContinueListEnabled(p_v); emit changed(); }
void EditorCfgQml::setAutoPair(bool p_v) { cfg().setAutoPairEnabled(p_v); emit changed(); }
