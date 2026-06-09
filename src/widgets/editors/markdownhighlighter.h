#ifndef MARKDOWNHIGHLIGHTER_H
#define MARKDOWNHIGHLIGHTER_H

#include <QColor>
#include <QObject>
#include <QQuickTextDocument>

namespace markly {
class MdHighlighter;

// Markdown syntax colors, sourced from the active theme.
struct MdColors {
  QColor heading;
  QColor emphasis;
  QColor codeInline;
  QColor link;
  QColor quote;
  QColor listMark;
  QColor codeInk;
  QColor mark;
};

// QML-instantiable bridge: set `document` to a TextEdit's textDocument and a
// QSyntaxHighlighter is attached. Colors follow the theme (re-highlights on change).
class MarkdownHighlighter : public QObject {
  Q_OBJECT
  Q_PROPERTY(QQuickTextDocument *document READ document WRITE setDocument NOTIFY documentChanged)
public:
  explicit MarkdownHighlighter(QObject *p_parent = nullptr);
  ~MarkdownHighlighter() override;

  QQuickTextDocument *document() const { return m_quickDoc; }
  void setDocument(QQuickTextDocument *p_doc);

signals:
  void documentChanged();

private:
  void refreshColors();

  QQuickTextDocument *m_quickDoc = nullptr;
  MdHighlighter *m_highlighter = nullptr;
  MdColors m_colors;
};
} // namespace markly

#endif // MARKDOWNHIGHLIGHTER_H
