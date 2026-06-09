#include "spellbridge.h"

#include <core/configmgr.h>
#include <core/editorconfig.h>
#include <core/marklyapp.h>
#include <core/spell/spellchecker.h>

using namespace markly;

SpellBridge::SpellBridge(QObject *p_parent) : QObject(p_parent) {}

bool SpellBridge::enabled() const {
  if (!ConfigMgr::getInst().getEditorConfig().isSpellCheckEnabled()) {
    return false;
  }
  auto *sc = MarklyApp::getInst().getSpellChecker();
  return sc && sc->ready();
}

bool SpellBridge::misspelled(const QString &p_word) const {
  if (!enabled() || p_word.isEmpty()) {
    return false;
  }
  return !MarklyApp::getInst().getSpellChecker()->check(p_word);
}

QStringList SpellBridge::suggest(const QString &p_word) const {
  auto *sc = MarklyApp::getInst().getSpellChecker();
  if (!sc || !sc->ready()) {
    return {};
  }
  QStringList s = sc->suggest(p_word);
  if (s.size() > 8) {
    s = s.mid(0, 8);
  }
  return s;
}

void SpellBridge::ignore(const QString &p_word) {
  auto *sc = MarklyApp::getInst().getSpellChecker();
  if (sc) {
    sc->addIgnore(p_word);
  }
  emit ignored();
}
