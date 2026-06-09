#include "vibridge.h"

#include <core/configmgr.h>
#include <core/editorconfig.h>

using namespace markly;

ViBridge::ViBridge(QObject *p_parent) : QObject(p_parent) {}

bool ViBridge::enabled() const {
  return ConfigMgr::getInst().getEditorConfig().isViModeEnabled();
}

QVariantMap ViBridge::handleKey(const QString &p_text, int p_cursor, int p_selStart,
                                int p_selEnd, const QString &p_keyText, int p_key, int p_mods) {
  const int before = static_cast<int>(m_engine.mode());
  ViResult res = m_engine.handleKey(p_text, p_cursor, p_selStart, p_selEnd, p_keyText, p_key, p_mods);

  QVariantList edits;
  for (const ViEdit &e : res.edits) {
    QVariantMap m;
    m[QStringLiteral("start")] = e.start;
    m[QStringLiteral("end")] = e.end;
    m[QStringLiteral("text")] = e.text;
    edits.append(m);
  }

  QVariantMap out;
  out[QStringLiteral("handled")] = res.handled;
  out[QStringLiteral("mode")] = static_cast<int>(res.mode);
  out[QStringLiteral("cursor")] = res.cursor;
  out[QStringLiteral("anchor")] = res.anchor;
  out[QStringLiteral("edits")] = edits;

  if (static_cast<int>(m_engine.mode()) != before) {
    emit modeChanged();
  }
  return out;
}

void ViBridge::reset() {
  const int before = static_cast<int>(m_engine.mode());
  m_engine.reset();
  if (static_cast<int>(m_engine.mode()) != before) {
    emit modeChanged();
  }
}
