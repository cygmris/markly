#include "viewarea.h"

#include <QFileInfo>
#include <QVariantMap>

#include <core/buffer/buffer.h>
#include <core/buffer/buffermgr.h>
#include <core/configmgr.h>
#include <core/exception.h>
#include <core/marklyapp.h>
#include <core/sessionconfig.h>

using namespace markly;

ViewArea::ViewArea(BufferMgr *p_bufferMgr, QObject *p_parent)
    : QObject(p_parent), m_bufferMgr(p_bufferMgr) {
  m_splits.append(Split{});
  const auto saved = ConfigMgr::getInst().getSessionConfig().getViewMode();
  if (saved == QStringLiteral("read") || saved == QStringLiteral("split")) {
    m_viewMode = saved;
  }
}

bool ViewArea::hasOpenFile() const {
  for (const auto &split : m_splits) {
    if (!split.m_tabs.isEmpty()) {
      return true;
    }
  }
  return false;
}

QVariantList ViewArea::splits() const {
  QVariantList list;
  for (int i = 0; i < m_splits.size(); ++i) {
    const auto &split = m_splits.at(i);
    QVariantMap sm;
    sm[QStringLiteral("index")] = i;
    sm[QStringLiteral("active")] = (i == m_activeSplit);
    sm[QStringLiteral("currentBufferId")] = static_cast<double>(split.m_active);

    QVariantList tabs;
    QString curName, curText;
    for (const auto id : split.m_tabs) {
      auto *buffer = m_bufferMgr ? m_bufferMgr->get(id) : nullptr;
      if (!buffer) {
        continue;
      }
      QVariantMap tm;
      tm[QStringLiteral("bufferId")] = static_cast<double>(id);
      tm[QStringLiteral("name")] = buffer->getName();
      tm[QStringLiteral("modified")] = buffer->isDirty();
      tm[QStringLiteral("active")] = (id == split.m_active);
      tabs.append(tm);
      if (id == split.m_active) {
        curName = buffer->getName();
        curText = buffer->getContent();
      }
    }
    sm[QStringLiteral("tabs")] = tabs;
    sm[QStringLiteral("currentName")] = curName;
    sm[QStringLiteral("currentText")] = curText;
    list.append(sm);
  }
  return list;
}

void ViewArea::openFile(const QString &p_path) {
  if (!m_bufferMgr) {
    return;
  }
  auto *buffer = m_bufferMgr->open(p_path);
  if (!buffer) {
    return;
  }
  auto &split = m_splits[m_activeSplit];
  if (!split.m_tabs.contains(buffer->getId())) {
    split.m_tabs.append(buffer->getId());
  }
  split.m_active = buffer->getId();
  persistSession();
  emit changed();
}

void ViewArea::activateTab(int p_splitIndex, double p_bufferId) {
  if (p_splitIndex < 0 || p_splitIndex >= m_splits.size()) {
    return;
  }
  m_splits[p_splitIndex].m_active = static_cast<ID>(p_bufferId);
  m_activeSplit = p_splitIndex;
  persistSession();
  emit changed();
}

void ViewArea::closeTab(int p_splitIndex, double p_bufferId) {
  if (p_splitIndex < 0 || p_splitIndex >= m_splits.size()) {
    return;
  }
  const auto id = static_cast<ID>(p_bufferId);
  auto &split = m_splits[p_splitIndex];
  split.m_tabs.removeAll(id);
  if (split.m_active == id) {
    split.m_active = split.m_tabs.isEmpty() ? 0 : split.m_tabs.last();
  }

  // Close the buffer if no split references it.
  bool referenced = false;
  for (const auto &s : m_splits) {
    if (s.m_tabs.contains(id)) {
      referenced = true;
      break;
    }
  }
  if (!referenced && m_bufferMgr) {
    m_bufferMgr->close(id);
  }

  removeEmptySplits();
  persistSession();
  emit changed();
}

void ViewArea::saveTab(double p_bufferId) {
  if (!m_bufferMgr) {
    return;
  }
  auto *buffer = m_bufferMgr->get(static_cast<ID>(p_bufferId));
  if (buffer) {
    bool saved = false;
    try {
      buffer->save();
      saved = true;
    } catch (Exception &e) {
      qWarning() << "saveTab failed" << e.what();
    }
    if (saved) {
      emit MarklyApp::getInst().noteSaved(buffer->getPath());
    }
    emit changed();
  }
}

void ViewArea::updateText(double p_bufferId, const QString &p_text) {
  if (!m_bufferMgr) {
    return;
  }
  auto *buffer = m_bufferMgr->get(static_cast<ID>(p_bufferId));
  if (buffer) {
    buffer->setContent(p_text);
    emit changed(); // updates the tab modified dot; QML keeps the editor text imperative
  }
}

QString ViewArea::textForBuffer(double p_bufferId) const {
  auto *buffer = m_bufferMgr ? m_bufferMgr->get(static_cast<ID>(p_bufferId)) : nullptr;
  return buffer ? buffer->getContent() : QString();
}

bool ViewArea::isTabDirty(double p_bufferId) const {
  auto *buffer = m_bufferMgr ? m_bufferMgr->get(static_cast<ID>(p_bufferId)) : nullptr;
  return buffer && buffer->isDirty();
}

void ViewArea::setStats(int p_line, int p_column, int p_lineCount, int p_charCount) {
  m_statsLine = p_line;
  m_statsColumn = p_column;
  m_statsLineCount = p_lineCount;
  m_statsCharCount = p_charCount;
  emit statsChanged();
}

void ViewArea::setViewMode(const QString &p_mode) {
  if (p_mode == m_viewMode) {
    return;
  }
  if (p_mode == QStringLiteral("edit") || p_mode == QStringLiteral("read") ||
      p_mode == QStringLiteral("split")) {
    m_viewMode = p_mode;
    ConfigMgr::getInst().getSessionConfig().setViewMode(m_viewMode);
    emit viewModeChanged();
  }
}

void ViewArea::cycleViewMode() {
  // edit -> split -> read -> edit
  if (m_viewMode == QStringLiteral("edit")) {
    setViewMode(QStringLiteral("split"));
  } else if (m_viewMode == QStringLiteral("split")) {
    setViewMode(QStringLiteral("read"));
  } else {
    setViewMode(QStringLiteral("edit"));
  }
}

void ViewArea::setActiveSplit(int p_index) {
  if (p_index >= 0 && p_index < m_splits.size()) {
    m_activeSplit = p_index;
    emit changed();
  }
}

void ViewArea::splitView() {
  if (m_splits.size() >= 2) {
    return; // basic spec supports up to 2 splits
  }
  m_splits.append(Split{});
  m_activeSplit = m_splits.size() - 1;
  emit changed();
}

void ViewArea::unsplit(int p_index) {
  if (m_splits.size() <= 1 || p_index < 0 || p_index >= m_splits.size()) {
    return;
  }
  // Close buffers only referenced by this split.
  const auto closing = m_splits[p_index].m_tabs;
  m_splits.removeAt(p_index);
  for (const auto id : closing) {
    bool referenced = false;
    for (const auto &s : m_splits) {
      if (s.m_tabs.contains(id)) {
        referenced = true;
        break;
      }
    }
    if (!referenced && m_bufferMgr) {
      m_bufferMgr->close(id);
    }
  }
  m_activeSplit = 0;
  persistSession();
  emit changed();
}

void ViewArea::removeEmptySplits() {
  for (int i = m_splits.size() - 1; i >= 0; --i) {
    if (m_splits.size() > 1 && m_splits.at(i).m_tabs.isEmpty()) {
      m_splits.removeAt(i);
    }
  }
  if (m_activeSplit >= m_splits.size()) {
    m_activeSplit = m_splits.size() - 1;
  }
}

void ViewArea::persistSession() {
  QStringList files;
  QString current;
  for (const auto &split : m_splits) {
    for (const auto id : split.m_tabs) {
      auto *buffer = m_bufferMgr ? m_bufferMgr->get(id) : nullptr;
      if (buffer && !files.contains(buffer->getPath())) {
        files.append(buffer->getPath());
      }
    }
  }
  if (m_activeSplit < m_splits.size()) {
    auto *buffer = m_bufferMgr ? m_bufferMgr->get(m_splits[m_activeSplit].m_active) : nullptr;
    if (buffer) {
      current = buffer->getPath();
    }
  }
  auto &session = ConfigMgr::getInst().getSessionConfig();
  session.setOpenedFiles(files);
  session.setCurrentFile(current);
}

void ViewArea::restoreSession() {
  auto &session = ConfigMgr::getInst().getSessionConfig();
  const auto files = session.getOpenedFiles();
  const auto current = session.getCurrentFile();
  for (const auto &path : files) {
    if (QFileInfo::exists(path)) {
      openFile(path);
    }
  }
  if (!current.isEmpty() && QFileInfo::exists(current)) {
    openFile(current); // makes it active in split 0
  }
}
