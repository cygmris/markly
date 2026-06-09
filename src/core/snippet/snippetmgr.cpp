#include "snippetmgr.h"

#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTime>
#include <QUuid>

using namespace markly;

SnippetMgr::SnippetMgr(QObject *p_parent) : QObject(p_parent) {}

QString SnippetMgr::dir() const {
  return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
         QStringLiteral("/snippets");
}

QString SnippetMgr::filePath(const QString &p_name) const {
  return dir() + QLatin1Char('/') + p_name + QStringLiteral(".json");
}

void SnippetMgr::load() {
  m_snippets.clear();
  QDir d(dir());
  if (!d.exists()) {
    seedDefaults();
    d.refresh();
  }
  const auto files = d.entryList({QStringLiteral("*.json")}, QDir::Files);
  for (const auto &file : files) {
    QFile f(d.absoluteFilePath(file));
    if (!f.open(QIODevice::ReadOnly)) {
      continue;
    }
    const auto obj = QJsonDocument::fromJson(f.readAll()).object();
    Snippet s;
    s.m_name = QFileInfo(file).completeBaseName();
    s.m_description = obj.value(QStringLiteral("description")).toString();
    s.m_content = obj.value(QStringLiteral("content")).toString();
    s.m_cursorMark = obj.value(QStringLiteral("cursor_mark")).toString(QStringLiteral("@@"));
    s.m_selectionMark =
        obj.value(QStringLiteral("selection_mark")).toString(QStringLiteral("$$"));
    m_snippets.insert(s.m_name, s);
  }
  emit changed();
}

void SnippetMgr::seedDefaults() {
  QDir().mkpath(dir());
  Snippet date;
  date.m_name = QStringLiteral("日期");
  date.m_description = QStringLiteral("插入当前日期");
  date.m_content = QStringLiteral("%date%");
  writeFile(date);

  Snippet code;
  code.m_name = QStringLiteral("代码块");
  code.m_description = QStringLiteral("围栏代码块");
  code.m_content = QStringLiteral("```@@\n$$\n```\n");
  writeFile(code);
}

void SnippetMgr::writeFile(const Snippet &p_snippet) const {
  QJsonObject obj;
  obj[QStringLiteral("description")] = p_snippet.m_description;
  obj[QStringLiteral("content")] = p_snippet.m_content;
  obj[QStringLiteral("cursor_mark")] = p_snippet.m_cursorMark;
  obj[QStringLiteral("selection_mark")] = p_snippet.m_selectionMark;
  QDir().mkpath(dir());
  QFile f(filePath(p_snippet.m_name));
  if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    f.write(QJsonDocument(obj).toJson());
    f.close();
  }
}

QList<Snippet> SnippetMgr::snippets() const {
  auto list = m_snippets.values();
  std::sort(list.begin(), list.end(),
            [](const Snippet &a, const Snippet &b) { return a.m_name < b.m_name; });
  return list;
}

void SnippetMgr::add(const QString &p_name, const QString &p_content) {
  if (p_name.isEmpty()) {
    return;
  }
  Snippet s;
  s.m_name = p_name;
  s.m_content = p_content;
  writeFile(s);
  m_snippets.insert(p_name, s);
  emit changed();
}

void SnippetMgr::remove(const QString &p_name) {
  if (m_snippets.contains(p_name)) {
    QFile::remove(filePath(p_name));
    m_snippets.remove(p_name);
    emit changed();
  }
}

QString SnippetMgr::expandMagicWords(const QString &p_text, const QString &p_noteName) {
  const auto now = QDateTime::currentDateTime();
  QString out = p_text;
  out.replace(QStringLiteral("%date%"), now.date().toString(Qt::ISODate));
  out.replace(QStringLiteral("%time%"), now.time().toString(QStringLiteral("HH:mm:ss")));
  out.replace(QStringLiteral("%datetime%"), now.toString(Qt::ISODate));
  out.replace(QStringLiteral("%note%"), p_noteName);
  // Replace each %uuid% with a distinct uuid.
  while (out.contains(QStringLiteral("%uuid%"))) {
    out.replace(out.indexOf(QStringLiteral("%uuid%")), 6,
                QUuid::createUuid().toString(QUuid::WithoutBraces));
  }
  return out;
}

QString SnippetMgr::apply(const QString &p_name, const QString &p_selectedText,
                          const QString &p_noteName, int &p_cursorOffset) const {
  p_cursorOffset = 0;
  if (!m_snippets.contains(p_name)) {
    return QString();
  }
  const auto s = m_snippets.value(p_name);
  QString text = expandMagicWords(s.m_content, p_noteName);
  if (!s.m_selectionMark.isEmpty()) {
    text.replace(s.m_selectionMark, p_selectedText);
  }
  if (!s.m_cursorMark.isEmpty()) {
    const int idx = text.indexOf(s.m_cursorMark);
    if (idx >= 0) {
      text.remove(idx, s.m_cursorMark.size());
      p_cursorOffset = idx;
    } else {
      p_cursorOffset = text.size();
    }
  } else {
    p_cursorOffset = text.size();
  }
  return text;
}
