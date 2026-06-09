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
