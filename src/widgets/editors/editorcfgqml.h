#ifndef EDITORCFGQML_H
#define EDITORCFGQML_H

#include <QObject>

namespace markly {
// QML bridge exposing EditorConfig fields (context property "EditorCfg"). Writable
// so the settings dialog (#19) can change editor options with immediate effect.
class EditorCfgQml : public QObject {
  Q_OBJECT
  Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY changed)
  Q_PROPERTY(int tabWidth READ tabWidth WRITE setTabWidth NOTIFY changed)
  Q_PROPERTY(bool expandTab READ expandTab WRITE setExpandTab NOTIFY changed)
  Q_PROPERTY(bool lineNumber READ lineNumber WRITE setLineNumber NOTIFY changed)
  Q_PROPERTY(bool highlightCurrentLine READ highlightCurrentLine WRITE setHighlightCurrentLine
                 NOTIFY changed)
  Q_PROPERTY(bool autoIndent READ autoIndent WRITE setAutoIndent NOTIFY changed)
  Q_PROPERTY(bool continueList READ continueList WRITE setContinueList NOTIFY changed)
  Q_PROPERTY(bool autoPair READ autoPair WRITE setAutoPair NOTIFY changed)
public:
  explicit EditorCfgQml(QObject *p_parent = nullptr);

  int fontSize() const;
  int tabWidth() const;
  bool expandTab() const;
  bool lineNumber() const;
  bool highlightCurrentLine() const;
  bool autoIndent() const;
  bool continueList() const;
  bool autoPair() const;

  void setFontSize(int p_v);
  void setTabWidth(int p_v);
  void setExpandTab(bool p_v);
  void setLineNumber(bool p_v);
  void setHighlightCurrentLine(bool p_v);
  void setAutoIndent(bool p_v);
  void setContinueList(bool p_v);
  void setAutoPair(bool p_v);

signals:
  void changed();
};
} // namespace markly

#endif // EDITORCFGQML_H
