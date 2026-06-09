#include "markdownhighlighter.h"

#include <QQuickTextDocument>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextDocument>

#include <core/marklyapp.h>
#include <core/thememgr.h>

using namespace markly;

namespace markly {
// Internal QSyntaxHighlighter implementing the markdown rules.
class MdHighlighter : public QSyntaxHighlighter {
public:
  MdHighlighter(QTextDocument *p_doc, const MdColors *p_colors)
      : QSyntaxHighlighter(p_doc), m_colors(p_colors) {}

protected:
  void highlightBlock(const QString &p_text) override {
    setCurrentBlockState(0);

    // Fenced code block tracking.
    const bool wasInCode = previousBlockState() == 1;
    const bool fence = p_text.trimmed().startsWith(QStringLiteral("```"));
    if (wasInCode) {
      QTextCharFormat fmt;
      fmt.setForeground(m_colors->codeInk);
      setFormat(0, p_text.length(), fmt);
      setCurrentBlockState(fence ? 0 : 1); // closing fence ends the block
      return;
    }
    if (fence) {
      QTextCharFormat fmt;
      fmt.setForeground(m_colors->codeInk);
      setFormat(0, p_text.length(), fmt);
      setCurrentBlockState(1);
      return;
    }

    // Heading.
    static const QRegularExpression heading(QStringLiteral("^(#{1,6})\\s+.*$"));
    auto hm = heading.match(p_text);
    if (hm.hasMatch()) {
      QTextCharFormat fmt;
      fmt.setForeground(m_colors->heading);
      fmt.setFontWeight(QFont::Bold);
      setFormat(0, p_text.length(), fmt);
      return;
    }

    // Quote.
    if (p_text.startsWith(QStringLiteral(">"))) {
      QTextCharFormat fmt;
      fmt.setForeground(m_colors->quote);
      fmt.setFontItalic(true);
      setFormat(0, p_text.length(), fmt);
      // still allow inline rules below for content
    }

    // List marker.
    static const QRegularExpression listMark(QStringLiteral("^(\\s*)([-*+]|\\d+\\.)\\s"));
    auto lm = listMark.match(p_text);
    if (lm.hasMatch()) {
      QTextCharFormat fmt;
      fmt.setForeground(m_colors->listMark);
      fmt.setFontWeight(QFont::Bold);
      setFormat(lm.capturedStart(2), lm.capturedLength(2), fmt);
    }

    applyInline(p_text, QStringLiteral("\\*\\*[^*\\n]+\\*\\*"), m_colors->emphasis, QFont::Bold, false);
    applyInline(p_text, QStringLiteral("(?<![\\*_])([*_])[^*_\\n]+\\1(?![\\*_])"), m_colors->emphasis, QFont::Normal, true);
    applyInline(p_text, QStringLiteral("~~[^~\\n]+~~"), m_colors->quote, QFont::Normal, false);
    applyInline(p_text, QStringLiteral("==[^=\\n]+=="), m_colors->mark, QFont::Normal, false);
    applyInline(p_text, QStringLiteral("`[^`\\n]+`"), m_colors->codeInline, QFont::Normal, false);
    applyInline(p_text, QStringLiteral("\\[[^\\]\\n]+\\]\\([^)\\n]+\\)"), m_colors->link, QFont::Normal, false);
  }

private:
  void applyInline(const QString &p_text, const QString &p_pattern, const QColor &p_color,
                   QFont::Weight p_weight, bool p_italic) {
    QRegularExpression re(p_pattern);
    auto it = re.globalMatch(p_text);
    while (it.hasNext()) {
      auto m = it.next();
      QTextCharFormat fmt;
      fmt.setForeground(p_color);
      if (p_weight == QFont::Bold) {
        fmt.setFontWeight(QFont::Bold);
      }
      if (p_italic) {
        fmt.setFontItalic(true);
      }
      setFormat(m.capturedStart(), m.capturedLength(), fmt);
    }
  }

  const MdColors *m_colors;
};
} // namespace markly

MarkdownHighlighter::MarkdownHighlighter(QObject *p_parent) : QObject(p_parent) {
  refreshColors();
  connect(&MarklyApp::getInst().getThemeMgr(), &ThemeMgr::themeChanged, this, [this]() {
    refreshColors();
    if (m_highlighter) {
      m_highlighter->rehighlight();
    }
  });
}

MarkdownHighlighter::~MarkdownHighlighter() { delete m_highlighter; }

void MarkdownHighlighter::refreshColors() {
  auto &t = MarklyApp::getInst().getThemeMgr();
  m_colors.heading = t.heading();
  m_colors.emphasis = t.emphasis();
  m_colors.codeInline = t.codeInline();
  m_colors.link = t.link();
  m_colors.quote = t.dim();
  m_colors.listMark = t.accent();
  m_colors.codeInk = t.codeInk();
  m_colors.mark = t.accent();
}

void MarkdownHighlighter::setDocument(QQuickTextDocument *p_doc) {
  if (m_quickDoc == p_doc) {
    return;
  }
  m_quickDoc = p_doc;
  delete m_highlighter;
  m_highlighter = nullptr;
  if (p_doc && p_doc->textDocument()) {
    m_highlighter = new MdHighlighter(p_doc->textDocument(), &m_colors);
  }
  emit documentChanged();
}
