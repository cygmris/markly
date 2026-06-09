#ifndef VXNODECONFIG_H
#define VXNODECONFIG_H

#include <QDateTime>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVector>

#include <core/global.h>

namespace markly {
// Serialization structs for a folder's vx.json (VNote-compatible).
namespace vx_node_config {

// A file child entry inside the parent folder's vx.json "files" array.
struct NodeFileConfig {
  QString m_name;
  ID m_id = 0;
  ID m_signature = 0;
  QDateTime m_createdTimeUtc;
  QDateTime m_modifiedTimeUtc;
  QString m_attachmentFolder;
  QStringList m_tags;
  QString m_backgroundColor;
  QString m_borderColor;
  QString m_nameColor;

  QJsonObject toJson() const;
  void fromJson(const QJsonObject &p_jobj);
};

// A folder child entry inside the parent folder's vx.json "folders" array.
struct NodeFolderConfig {
  QString m_name;
  QString m_backgroundColor;
  QString m_borderColor;
  QString m_nameColor;

  QJsonObject toJson() const;
  void fromJson(const QJsonObject &p_jobj);
};

// A folder's own vx.json (top level).
struct NodeConfig {
  int m_version = 1;
  ID m_id = 0;
  ID m_signature = 0;
  QDateTime m_createdTimeUtc;
  QDateTime m_modifiedTimeUtc;
  QString m_backgroundColor;
  QString m_borderColor;
  QString m_nameColor;
  QVector<NodeFileConfig> m_files;
  QVector<NodeFolderConfig> m_folders;

  QJsonObject toJson() const;
  void fromJson(const QJsonObject &p_jobj);
};

} // namespace vx_node_config
} // namespace markly

#endif // VXNODECONFIG_H
