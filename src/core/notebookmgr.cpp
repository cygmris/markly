#include "notebookmgr.h"

#include <QDebug>

#include "configmgr.h"
#include "exception.h"
#include "notebook/notebook.h"
#include "notebookbackend/localnotebookbackend.h"
#include "sessionconfig.h"

using namespace markly;

NotebookMgr::NotebookMgr(QObject *p_parent) : QObject(p_parent) {}

QSharedPointer<Notebook> NotebookMgr::newNotebook(const QString &p_rootDir, const QString &p_name,
                                                  const QString &p_description) {
  auto backend = new LocalNotebookBackend(p_rootDir);
  auto notebook = QSharedPointer<Notebook>::create(m_nextId++, p_rootDir, backend);
  notebook->createOnDisk(p_name, p_description);
  m_notebooks.append(notebook);
  m_currentId = notebook->getId();
  persist();
  emit notebookAdded(notebook.data());
  emit currentNotebookChanged(notebook.data());
  return notebook;
}

QSharedPointer<Notebook> NotebookMgr::openNotebook(const QString &p_rootDir) {
  // Already open?
  for (const auto &nb : m_notebooks) {
    if (nb->getRootPath() == p_rootDir) {
      return nb;
    }
  }

  auto backend = new LocalNotebookBackend(p_rootDir);
  if (!backend->existsFile(Notebook::c_notebookConfigName)) {
    delete backend;
    qWarning() << "not a notebook directory (missing" << Notebook::c_notebookConfigName << "):"
               << p_rootDir;
    return nullptr;
  }
  auto notebook = QSharedPointer<Notebook>::create(m_nextId++, p_rootDir, backend);
  try {
    notebook->open();
  } catch (Exception &e) {
    qWarning() << "failed to open notebook" << p_rootDir << e.what();
    return nullptr;
  }
  m_notebooks.append(notebook);
  if (m_currentId == 0) {
    m_currentId = notebook->getId();
    emit currentNotebookChanged(notebook.data());
  }
  persist();
  emit notebookAdded(notebook.data());
  return notebook;
}

void NotebookMgr::closeNotebook(ID p_id) {
  for (int i = 0; i < m_notebooks.size(); ++i) {
    if (m_notebooks[i]->getId() == p_id) {
      m_notebooks.removeAt(i);
      if (m_currentId == p_id) {
        m_currentId = m_notebooks.isEmpty() ? 0 : m_notebooks.first()->getId();
        emit currentNotebookChanged(getCurrentNotebook().data());
      }
      persist();
      emit notebookRemoved(p_id);
      return;
    }
  }
}

QSharedPointer<Notebook> NotebookMgr::getNotebook(ID p_id) const {
  for (const auto &nb : m_notebooks) {
    if (nb->getId() == p_id) {
      return nb;
    }
  }
  return nullptr;
}

QSharedPointer<Notebook> NotebookMgr::getCurrentNotebook() const { return getNotebook(m_currentId); }

void NotebookMgr::setCurrentNotebook(ID p_id) {
  if (p_id == m_currentId) {
    return;
  }
  auto nb = getNotebook(p_id);
  if (!nb) {
    return;
  }
  m_currentId = p_id;
  persist();
  emit currentNotebookChanged(nb.data());
}

void NotebookMgr::loadNotebooks() {
  auto &session = ConfigMgr::getInst().getSessionConfig();
  const auto paths = session.getNotebookRootPaths();
  const auto currentPath = session.getCurrentNotebookRootPath();
  for (const auto &path : paths) {
    auto nb = openNotebook(path);
    if (nb && path == currentPath) {
      m_currentId = nb->getId();
    }
  }
  if (m_currentId == 0 && !m_notebooks.isEmpty()) {
    m_currentId = m_notebooks.first()->getId();
  }
  if (auto cur = getCurrentNotebook()) {
    emit currentNotebookChanged(cur.data());
  }
}

void NotebookMgr::persist() {
  QStringList paths;
  for (const auto &nb : m_notebooks) {
    paths.append(nb->getRootPath());
  }
  auto &session = ConfigMgr::getInst().getSessionConfig();
  session.setNotebookRootPaths(paths);
  auto cur = getCurrentNotebook();
  session.setCurrentNotebookRootPath(cur ? cur->getRootPath() : QString());
}
