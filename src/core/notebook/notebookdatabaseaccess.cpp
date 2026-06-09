#include "notebookdatabaseaccess.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

using namespace markly;

NotebookDatabaseAccess::NotebookDatabaseAccess(const QString &p_dbPath,
                                               const QString &p_connectionName)
    : m_dbPath(p_dbPath), m_connectionName(p_connectionName) {}

NotebookDatabaseAccess::~NotebookDatabaseAccess() { close(); }

bool NotebookDatabaseAccess::open() {
  if (m_open) {
    return true;
  }
  auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
  db.setDatabaseName(m_dbPath);
  if (!db.open()) {
    qWarning() << "failed to open notebook db" << m_dbPath << db.lastError().text();
    return false;
  }
  QSqlQuery query(db);
  query.exec(QStringLiteral("PRAGMA foreign_keys = ON"));
  m_open = true;
  return true;
}

void NotebookDatabaseAccess::close() {
  if (m_open) {
    {
      auto db = QSqlDatabase::database(m_connectionName);
      if (db.isOpen()) {
        db.close();
      }
    }
    QSqlDatabase::removeDatabase(m_connectionName);
    m_open = false;
  }
}

bool NotebookDatabaseAccess::createTables() {
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);

  bool ok = true;
  ok &= query.exec(QStringLiteral(
      "CREATE TABLE IF NOT EXISTS node ("
      "  id INTEGER PRIMARY KEY,"
      "  name TEXT NOT NULL,"
      "  signature INTEGER NOT NULL,"
      "  parent_id INTEGER,"
      "  FOREIGN KEY(parent_id) REFERENCES node(id) ON DELETE CASCADE)"));
  ok &= query.exec(QStringLiteral(
      "CREATE TABLE IF NOT EXISTS tag ("
      "  name TEXT PRIMARY KEY,"
      "  parent_name TEXT,"
      "  FOREIGN KEY(parent_name) REFERENCES tag(name) ON DELETE CASCADE) WITHOUT ROWID"));
  ok &= query.exec(QStringLiteral(
      "CREATE TABLE IF NOT EXISTS tag_node ("
      "  node_id INTEGER,"
      "  tag_name TEXT,"
      "  FOREIGN KEY(node_id) REFERENCES node(id) ON DELETE CASCADE,"
      "  FOREIGN KEY(tag_name) REFERENCES tag(name) ON DELETE CASCADE)"));

  if (!ok) {
    qWarning() << "failed to create notebook db tables" << query.lastError().text();
  }
  return ok;
}

ID NotebookDatabaseAccess::insertNode(const QString &p_name, ID p_signature, ID p_parentId) {
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  query.prepare(
      QStringLiteral("INSERT INTO node (name, signature, parent_id) VALUES (?, ?, ?)"));
  query.addBindValue(p_name);
  query.addBindValue(static_cast<qulonglong>(p_signature));
  if (p_parentId == 0) {
    query.addBindValue(QVariant());
  } else {
    query.addBindValue(static_cast<qulonglong>(p_parentId));
  }
  if (!query.exec()) {
    qWarning() << "insertNode failed" << query.lastError().text();
    return 0;
  }
  return query.lastInsertId().toULongLong();
}

bool NotebookDatabaseAccess::addNode(const NodeRow &p_row) {
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  query.prepare(QStringLiteral(
      "INSERT OR REPLACE INTO node (id, name, signature, parent_id) VALUES (?, ?, ?, ?)"));
  query.addBindValue(static_cast<qulonglong>(p_row.m_id));
  query.addBindValue(p_row.m_name);
  query.addBindValue(static_cast<qulonglong>(p_row.m_signature));
  if (p_row.m_parentId == 0) {
    query.addBindValue(QVariant());
  } else {
    query.addBindValue(static_cast<qulonglong>(p_row.m_parentId));
  }
  if (!query.exec()) {
    qWarning() << "addNode failed" << query.lastError().text();
    return false;
  }
  return true;
}

bool NotebookDatabaseAccess::updateNode(const NodeRow &p_row) { return addNode(p_row); }

bool NotebookDatabaseAccess::removeNode(ID p_id) {
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  query.prepare(QStringLiteral("DELETE FROM node WHERE id = ?"));
  query.addBindValue(static_cast<qulonglong>(p_id));
  return query.exec();
}

bool NotebookDatabaseAccess::nodeExists(ID p_id) const {
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  query.prepare(QStringLiteral("SELECT 1 FROM node WHERE id = ?"));
  query.addBindValue(static_cast<qulonglong>(p_id));
  return query.exec() && query.next();
}

QVector<NotebookDatabaseAccess::NodeRow> NotebookDatabaseAccess::queryChildren(ID p_parentId) const {
  QVector<NodeRow> rows;
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  if (p_parentId == 0) {
    query.prepare(QStringLiteral(
        "SELECT id, name, signature, parent_id FROM node WHERE parent_id IS NULL"));
  } else {
    query.prepare(
        QStringLiteral("SELECT id, name, signature, parent_id FROM node WHERE parent_id = ?"));
    query.addBindValue(static_cast<qulonglong>(p_parentId));
  }
  if (query.exec()) {
    while (query.next()) {
      NodeRow row;
      row.m_id = query.value(0).toULongLong();
      row.m_name = query.value(1).toString();
      row.m_signature = query.value(2).toULongLong();
      row.m_parentId = query.value(3).isNull() ? 0 : query.value(3).toULongLong();
      rows.append(row);
    }
  }
  return rows;
}

bool NotebookDatabaseAccess::addTag(const QString &p_name, const QString &p_parentName) {
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  query.prepare(QStringLiteral("INSERT OR REPLACE INTO tag (name, parent_name) VALUES (?, ?)"));
  query.addBindValue(p_name);
  query.addBindValue(p_parentName.isEmpty() ? QVariant() : QVariant(p_parentName));
  return query.exec();
}

bool NotebookDatabaseAccess::removeTag(const QString &p_name) {
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  query.prepare(QStringLiteral("DELETE FROM tag WHERE name = ?"));
  query.addBindValue(p_name);
  return query.exec();
}

QStringList NotebookDatabaseAccess::queryAllTags() const {
  QStringList tags;
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  if (query.exec(QStringLiteral("SELECT name FROM tag"))) {
    while (query.next()) {
      tags.append(query.value(0).toString());
    }
  }
  return tags;
}

bool NotebookDatabaseAccess::addNodeTag(ID p_nodeId, const QString &p_tagName) {
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  query.prepare(QStringLiteral("INSERT INTO tag_node (node_id, tag_name) VALUES (?, ?)"));
  query.addBindValue(static_cast<qulonglong>(p_nodeId));
  query.addBindValue(p_tagName);
  return query.exec();
}

bool NotebookDatabaseAccess::removeNodeTags(ID p_nodeId) {
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  query.prepare(QStringLiteral("DELETE FROM tag_node WHERE node_id = ?"));
  query.addBindValue(static_cast<qulonglong>(p_nodeId));
  return query.exec();
}

QStringList NotebookDatabaseAccess::queryNodeTags(ID p_nodeId) const {
  QStringList tags;
  auto db = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(db);
  query.prepare(QStringLiteral("SELECT tag_name FROM tag_node WHERE node_id = ?"));
  query.addBindValue(static_cast<qulonglong>(p_nodeId));
  if (query.exec()) {
    while (query.next()) {
      tags.append(query.value(0).toString());
    }
  }
  return tags;
}
