#include "editorconfig.h"

using namespace markly;

EditorConfig::EditorConfig(ConfigMgr *p_mgr, IConfig *p_topConfig)
    : IConfig(p_mgr, p_topConfig) {}

void EditorConfig::init(const QJsonObject &p_app, const QJsonObject &p_user) {
  m_sessionName = QStringLiteral("editor");

  // Merge: start from app defaults, override with user keys.
  m_jobj = p_app;
  for (auto it = p_user.begin(); it != p_user.end(); ++it) {
    m_jobj[it.key()] = it.value();
  }
}

QJsonObject EditorConfig::toJson() const { return m_jobj; }
