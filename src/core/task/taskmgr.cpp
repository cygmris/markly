#include "taskmgr.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QStandardPaths>

using namespace markly;

TaskMgr::TaskMgr(QObject *p_parent) : QObject(p_parent) {}

QString TaskMgr::dir() const {
  return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
         QStringLiteral("/tasks");
}

void TaskMgr::load() {
  m_tasks.clear();
  QDir d(dir());
  if (!d.exists()) {
    seedDefaults();
    d.refresh();
  }
  for (const auto &file : d.entryList({QStringLiteral("*.json")}, QDir::Files)) {
    QFile f(d.absoluteFilePath(file));
    if (!f.open(QIODevice::ReadOnly)) {
      continue;
    }
    const auto obj = QJsonDocument::fromJson(f.readAll()).object();
    Task t;
    t.m_name = QFileInfo(file).completeBaseName();
    t.m_command = obj.value(QStringLiteral("command")).toString();
    for (const auto &a : obj.value(QStringLiteral("args")).toArray()) {
      t.m_args.append(a.toString());
    }
    t.m_shell = obj.value(QStringLiteral("shell")).toBool(true);
    m_tasks.insert(t.m_name, t);
  }
  emit changed();
}

void TaskMgr::seedDefaults() {
  QDir().mkpath(dir());
  QJsonObject obj;
  obj[QStringLiteral("command")] = QStringLiteral("wc -m \"${bufferPath}\"");
  obj[QStringLiteral("shell")] = true;
  QFile f(dir() + QStringLiteral("/字数统计.json"));
  if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    f.write(QJsonDocument(obj).toJson());
    f.close();
  }
}

QList<Task> TaskMgr::tasks() const {
  auto list = m_tasks.values();
  std::sort(list.begin(), list.end(),
            [](const Task &a, const Task &b) { return a.m_name < b.m_name; });
  return list;
}

QString TaskMgr::expand(const QString &p_text, const TaskContext &p_ctx) {
  QString out = p_text;
  out.replace(QStringLiteral("${bufferPath}"), p_ctx.m_bufferPath);
  out.replace(QStringLiteral("${bufferDir}"), p_ctx.m_bufferDir);
  out.replace(QStringLiteral("${bufferName}"), p_ctx.m_bufferName);
  out.replace(QStringLiteral("${bufferBaseName}"),
              QFileInfo(p_ctx.m_bufferName).completeBaseName());
  out.replace(QStringLiteral("${notebookFolder}"), p_ctx.m_notebookFolder);
  const auto cwd = p_ctx.m_notebookFolder.isEmpty() ? p_ctx.m_bufferDir : p_ctx.m_notebookFolder;
  out.replace(QStringLiteral("${cwd}"), cwd);
  return out;
}

QString TaskMgr::run(const QString &p_name, const TaskContext &p_ctx, int &p_outExit,
                     int p_timeoutMs) const {
  p_outExit = -1;
  if (!m_tasks.contains(p_name)) {
    return QStringLiteral("任务不存在: %1").arg(p_name);
  }
  const auto task = m_tasks.value(p_name);
  const auto cwd =
      p_ctx.m_notebookFolder.isEmpty() ? p_ctx.m_bufferDir : p_ctx.m_notebookFolder;

  QProcess proc;
  if (!cwd.isEmpty()) {
    proc.setWorkingDirectory(cwd);
  }
  proc.setProcessChannelMode(QProcess::MergedChannels);

  if (task.m_shell) {
    const auto cmd = expand(task.m_command, p_ctx);
    proc.start(QStringLiteral("sh"), {QStringLiteral("-c"), cmd});
  } else {
    QStringList args;
    for (const auto &a : task.m_args) {
      args.append(expand(a, p_ctx));
    }
    proc.start(expand(task.m_command, p_ctx), args);
  }

  if (!proc.waitForStarted(3000)) {
    return QStringLiteral("无法启动任务: %1").arg(expand(task.m_command, p_ctx));
  }
  if (!proc.waitForFinished(p_timeoutMs)) {
    proc.kill();
    proc.waitForFinished(1000);
    return QStringLiteral("任务超时（>%1ms）").arg(p_timeoutMs);
  }
  p_outExit = proc.exitCode();
  return QString::fromUtf8(proc.readAll());
}
