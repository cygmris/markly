#ifndef NOTEBOOKCONFIG_H
#define NOTEBOOKCONFIG_H

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

namespace markly {
// Serialization for a notebook's vx_notebook.json (VNote-compatible).
// History is preserved as raw JSON (modeled in the history spec #13).
struct NotebookConfig {
  int m_version = 3;
  QString m_name;
  QString m_description;
  QString m_imageFolder = QStringLiteral("vx_images");
  QString m_attachmentFolder = QStringLiteral("vx_attachments");
  QDateTime m_createdTimeUtc;
  QString m_versionController = QStringLiteral("dummy.vnotex");
  QString m_notebookConfigMgr = QStringLiteral("vx.vnotex");
  QJsonArray m_history;
  // Tag graph like "parent>child;parent2>child2".
  QString m_tagGraph;
  QJsonObject m_extraConfigs;

  QJsonObject toJson() const;
  void fromJson(const QJsonObject &p_jobj);
};
} // namespace markly

#endif // NOTEBOOKCONFIG_H
