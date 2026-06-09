#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H

#include "iconfig.h"

namespace markly {
// Editor configuration framework.
// NOTE: foundation provides only the IConfig scaffolding; the markdown-editor specs
// (#7/#8) add concrete fields. Unknown keys are round-tripped to avoid data loss.
class EditorConfig : public IConfig {
public:
  EditorConfig(ConfigMgr *p_mgr, IConfig *p_topConfig);

  void init(const QJsonObject &p_app, const QJsonObject &p_user) Q_DECL_OVERRIDE;

  QJsonObject toJson() const Q_DECL_OVERRIDE;

private:
  // Merged (user over app) raw section, round-tripped until concrete fields land.
  QJsonObject m_jobj;
};
} // namespace markly

#endif // EDITORCONFIG_H
