#ifndef EDITORCFGQML_H
#define EDITORCFGQML_H

#include <QObject>

namespace markly {
// QML bridge exposing EditorConfig fields (context property "EditorCfg").
class EditorCfgQml : public QObject {
  Q_OBJECT
  Q_PROPERTY(int fontSize READ fontSize NOTIFY changed)
  Q_PROPERTY(int tabWidth READ tabWidth NOTIFY changed)
  Q_PROPERTY(bool expandTab READ expandTab NOTIFY changed)
  Q_PROPERTY(bool lineNumber READ lineNumber NOTIFY changed)
  Q_PROPERTY(bool highlightCurrentLine READ highlightCurrentLine NOTIFY changed)
  Q_PROPERTY(bool autoIndent READ autoIndent NOTIFY changed)
  Q_PROPERTY(bool continueList READ continueList NOTIFY changed)
  Q_PROPERTY(bool autoPair READ autoPair NOTIFY changed)
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

signals:
  void changed();
};
} // namespace markly

#endif // EDITORCFGQML_H
