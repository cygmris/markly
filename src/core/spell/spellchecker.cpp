#include "spellchecker.h"

#include <QFile>
#include <QStringConverter>

#include <hunspell/hunspell.h>

using namespace markly;

namespace {
// Map the dictionary's declared charset to a Qt6 QStringConverter encoding.
// Defaults to UTF-8 (the en_US dictionary's `SET UTF-8`); ISO-8859-1 supported
// for legacy dictionaries. Unknown -> UTF-8.
QStringConverter::Encoding encodingFor(const QByteArray &p_name) {
  if (p_name.compare("ISO8859-1", Qt::CaseInsensitive) == 0 ||
      p_name.compare("ISO-8859-1", Qt::CaseInsensitive) == 0 ||
      p_name.compare("Latin1", Qt::CaseInsensitive) == 0) {
    return QStringConverter::Latin1;
  }
  return QStringConverter::Utf8;
}
} // namespace

SpellChecker::SpellChecker(const QString &p_dictDir, const QString &p_lang) {
  const QString aff = p_dictDir + QStringLiteral("/") + p_lang + QStringLiteral(".aff");
  const QString dic = p_dictDir + QStringLiteral("/") + p_lang + QStringLiteral(".dic");
  if (!QFile::exists(aff) || !QFile::exists(dic)) {
    return; // not ready -> safe degrade
  }
  m_handle = Hunspell_create(aff.toLocal8Bit().constData(), dic.toLocal8Bit().constData());
  if (m_handle) {
    m_encoding = QByteArray(Hunspell_get_dic_encoding(static_cast<Hunhandle *>(m_handle)));
  }
}

SpellChecker::~SpellChecker() {
  if (m_handle) {
    Hunspell_destroy(static_cast<Hunhandle *>(m_handle));
    m_handle = nullptr;
  }
}

void SpellChecker::addIgnore(const QString &p_word) {
  if (!p_word.isEmpty()) {
    m_ignore.insert(p_word.toLower());
  }
}

bool SpellChecker::isIgnored(const QString &p_word) const {
  return m_ignore.contains(p_word.toLower());
}

bool SpellChecker::check(const QString &p_word) const {
  if (!m_handle || p_word.isEmpty() || isIgnored(p_word)) {
    return true;
  }
  QStringEncoder enc(encodingFor(m_encoding));
  const QByteArray bytes = enc.encode(p_word);
  // Hunspell_spell returns non-zero when the word is recognized.
  return Hunspell_spell(static_cast<Hunhandle *>(m_handle), bytes.constData()) != 0;
}

QStringList SpellChecker::suggest(const QString &p_word) const {
  QStringList result;
  if (!m_handle || p_word.isEmpty()) {
    return result;
  }
  const QStringConverter::Encoding e = encodingFor(m_encoding);
  QStringEncoder enc(e);
  const QByteArray bytes = enc.encode(p_word);
  char **list = nullptr;
  const int n = Hunspell_suggest(static_cast<Hunhandle *>(m_handle), &list, bytes.constData());
  QStringDecoder dec(e);
  for (int i = 0; i < n; ++i) {
    result << dec.decode(QByteArray(list[i]));
  }
  if (list) {
    Hunspell_free_list(static_cast<Hunhandle *>(m_handle), &list, n);
  }
  return result;
}
