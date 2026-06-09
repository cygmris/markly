#include "vxnodeconfig.h"

#include <QJsonArray>

using namespace markly;
using namespace markly::vx_node_config;

namespace {
QString timeToStr(const QDateTime &p_time) {
  return p_time.toUTC().toString(Qt::ISODate);
}
QDateTime strToTime(const QString &p_str) {
  return QDateTime::fromString(p_str, Qt::ISODate).toUTC();
}
ID readId(const QJsonObject &p_obj, const QString &p_key) {
  // ids/signatures can exceed int range; read as double/string defensively.
  const auto v = p_obj.value(p_key);
  if (v.isString()) {
    return v.toString().toULongLong();
  }
  return static_cast<ID>(v.toDouble());
}
QStringList readStrList(const QJsonObject &p_obj, const QString &p_key) {
  QStringList res;
  for (const auto &v : p_obj.value(p_key).toArray()) {
    res.append(v.toString());
  }
  return res;
}
} // namespace

// ---- NodeFileConfig ----
QJsonObject NodeFileConfig::toJson() const {
  QJsonObject obj;
  obj[QStringLiteral("name")] = m_name;
  obj[QStringLiteral("id")] = static_cast<double>(m_id);
  obj[QStringLiteral("signature")] = static_cast<double>(m_signature);
  obj[QStringLiteral("created_time")] = timeToStr(m_createdTimeUtc);
  obj[QStringLiteral("modified_time")] = timeToStr(m_modifiedTimeUtc);
  obj[QStringLiteral("attachment_folder")] = m_attachmentFolder;
  QJsonArray tags;
  for (const auto &t : m_tags) {
    tags.append(t);
  }
  obj[QStringLiteral("tags")] = tags;
  obj[QStringLiteral("background_color")] = m_backgroundColor;
  obj[QStringLiteral("border_color")] = m_borderColor;
  obj[QStringLiteral("name_color")] = m_nameColor;
  return obj;
}

void NodeFileConfig::fromJson(const QJsonObject &p_jobj) {
  m_name = p_jobj.value(QStringLiteral("name")).toString();
  m_id = readId(p_jobj, QStringLiteral("id"));
  m_signature = readId(p_jobj, QStringLiteral("signature"));
  m_createdTimeUtc = strToTime(p_jobj.value(QStringLiteral("created_time")).toString());
  m_modifiedTimeUtc = strToTime(p_jobj.value(QStringLiteral("modified_time")).toString());
  m_attachmentFolder = p_jobj.value(QStringLiteral("attachment_folder")).toString();
  m_tags = readStrList(p_jobj, QStringLiteral("tags"));
  m_backgroundColor = p_jobj.value(QStringLiteral("background_color")).toString();
  m_borderColor = p_jobj.value(QStringLiteral("border_color")).toString();
  m_nameColor = p_jobj.value(QStringLiteral("name_color")).toString();
}

// ---- NodeFolderConfig ----
QJsonObject NodeFolderConfig::toJson() const {
  QJsonObject obj;
  obj[QStringLiteral("name")] = m_name;
  obj[QStringLiteral("background_color")] = m_backgroundColor;
  obj[QStringLiteral("border_color")] = m_borderColor;
  obj[QStringLiteral("name_color")] = m_nameColor;
  return obj;
}

void NodeFolderConfig::fromJson(const QJsonObject &p_jobj) {
  m_name = p_jobj.value(QStringLiteral("name")).toString();
  m_backgroundColor = p_jobj.value(QStringLiteral("background_color")).toString();
  m_borderColor = p_jobj.value(QStringLiteral("border_color")).toString();
  m_nameColor = p_jobj.value(QStringLiteral("name_color")).toString();
}

// ---- NodeConfig ----
QJsonObject NodeConfig::toJson() const {
  QJsonObject obj;
  obj[QStringLiteral("version")] = m_version;
  obj[QStringLiteral("id")] = static_cast<double>(m_id);
  obj[QStringLiteral("signature")] = static_cast<double>(m_signature);
  obj[QStringLiteral("created_time")] = timeToStr(m_createdTimeUtc);
  obj[QStringLiteral("modified_time")] = timeToStr(m_modifiedTimeUtc);
  obj[QStringLiteral("background_color")] = m_backgroundColor;
  obj[QStringLiteral("border_color")] = m_borderColor;
  obj[QStringLiteral("name_color")] = m_nameColor;
  QJsonArray files;
  for (const auto &f : m_files) {
    files.append(f.toJson());
  }
  obj[QStringLiteral("files")] = files;
  QJsonArray folders;
  for (const auto &f : m_folders) {
    folders.append(f.toJson());
  }
  obj[QStringLiteral("folders")] = folders;
  return obj;
}

void NodeConfig::fromJson(const QJsonObject &p_jobj) {
  m_version = p_jobj.value(QStringLiteral("version")).toInt(1);
  m_id = readId(p_jobj, QStringLiteral("id"));
  m_signature = readId(p_jobj, QStringLiteral("signature"));
  m_createdTimeUtc = strToTime(p_jobj.value(QStringLiteral("created_time")).toString());
  m_modifiedTimeUtc = strToTime(p_jobj.value(QStringLiteral("modified_time")).toString());
  m_backgroundColor = p_jobj.value(QStringLiteral("background_color")).toString();
  m_borderColor = p_jobj.value(QStringLiteral("border_color")).toString();
  m_nameColor = p_jobj.value(QStringLiteral("name_color")).toString();

  m_files.clear();
  for (const auto &v : p_jobj.value(QStringLiteral("files")).toArray()) {
    NodeFileConfig f;
    f.fromJson(v.toObject());
    m_files.append(f);
  }
  m_folders.clear();
  for (const auto &v : p_jobj.value(QStringLiteral("folders")).toArray()) {
    NodeFolderConfig f;
    f.fromJson(v.toObject());
    m_folders.append(f);
  }
}
