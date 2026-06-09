#include "notebookconfig.h"

using namespace markly;

QJsonObject NotebookConfig::toJson() const {
  QJsonObject obj;
  obj[QStringLiteral("version")] = m_version;
  obj[QStringLiteral("name")] = m_name;
  obj[QStringLiteral("description")] = m_description;
  obj[QStringLiteral("image_folder")] = m_imageFolder;
  obj[QStringLiteral("attachment_folder")] = m_attachmentFolder;
  obj[QStringLiteral("created_time")] = m_createdTimeUtc.toUTC().toString(Qt::ISODate);
  obj[QStringLiteral("version_controller")] = m_versionController;
  obj[QStringLiteral("config_mgr")] = m_notebookConfigMgr;
  obj[QStringLiteral("history")] = m_history;
  obj[QStringLiteral("tag_graph")] = m_tagGraph;
  obj[QStringLiteral("extra_configs")] = m_extraConfigs;
  return obj;
}

void NotebookConfig::fromJson(const QJsonObject &p_jobj) {
  m_version = p_jobj.value(QStringLiteral("version")).toInt(3);
  m_name = p_jobj.value(QStringLiteral("name")).toString();
  m_description = p_jobj.value(QStringLiteral("description")).toString();
  m_imageFolder = p_jobj.value(QStringLiteral("image_folder")).toString(QStringLiteral("vx_images"));
  m_attachmentFolder =
      p_jobj.value(QStringLiteral("attachment_folder")).toString(QStringLiteral("vx_attachments"));
  m_createdTimeUtc =
      QDateTime::fromString(p_jobj.value(QStringLiteral("created_time")).toString(), Qt::ISODate)
          .toUTC();
  m_versionController = p_jobj.value(QStringLiteral("version_controller"))
                            .toString(QStringLiteral("dummy.vnotex"));
  m_notebookConfigMgr =
      p_jobj.value(QStringLiteral("config_mgr")).toString(QStringLiteral("vx.vnotex"));
  m_history = p_jobj.value(QStringLiteral("history")).toArray();
  m_tagGraph = p_jobj.value(QStringLiteral("tag_graph")).toString();
  m_extraConfigs = p_jobj.value(QStringLiteral("extra_configs")).toObject();
}
