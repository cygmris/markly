#include "testbridge.h"

#include <QApplication>
#include <QEventLoop>
#include <QFile>
#include <QImage>
#include <QLocalServer>
#include <QLocalSocket>
#include <QQuickItem>
#include <QQuickWidget>
#include <QTimer>

using namespace markly;

TestBridge::TestBridge(QQuickWidget *p_quick, QObject *p_parent)
    : QObject(p_parent), m_quick(p_quick) {}

bool TestBridge::start(const QString &p_socketPath) {
  QLocalServer::removeServer(p_socketPath); // clear stale socket
  m_server = new QLocalServer(this);
  if (!m_server->listen(p_socketPath)) {
    qWarning() << "TestBridge: listen failed" << p_socketPath << m_server->errorString();
    return false;
  }
  connect(m_server, &QLocalServer::newConnection, this, &TestBridge::onNewConnection);
  qInfo() << "TestBridge listening on" << p_socketPath;
  return true;
}

void TestBridge::onNewConnection() {
  auto *sock = m_server->nextPendingConnection();
  if (!sock) return;
  m_client = sock;
  connect(sock, &QLocalSocket::readyRead, this, &TestBridge::onReadyRead);
  connect(sock, &QLocalSocket::disconnected, sock, &QObject::deleteLater);
}

void TestBridge::onReadyRead() {
  auto *sock = qobject_cast<QLocalSocket *>(sender());
  if (!sock) return;
  while (sock->canReadLine()) {
    const auto line = QString::fromUtf8(sock->readLine()).trimmed();
    if (line.isEmpty()) continue;
    const auto reply = handle(line);
    sock->write(reply.toUtf8());
    sock->write("\n");
    sock->flush();
  }
}

QString TestBridge::handle(const QString &p_line) {
  const int sp = p_line.indexOf(' ');
  const QString verb = sp < 0 ? p_line : p_line.left(sp);
  const QString arg = sp < 0 ? QString() : p_line.mid(sp + 1);
  auto *root = m_quick ? m_quick->rootObject() : nullptr;
  if (!root && verb != QStringLiteral("quit")) return QStringLiteral("ERR no root");

  if (verb == QStringLiteral("dialog")) {
    QMetaObject::invokeMethod(root, "showDialog", Q_ARG(QVariant, arg));
    return QStringLiteral("OK");
  }
  if (verb == QStringLiteral("content")) {
    QMetaObject::invokeMethod(root, "testContent", Q_ARG(QVariant, arg));
    return QStringLiteral("OK");
  }
  if (verb == QStringLiteral("viewmode")) {
    QMetaObject::invokeMethod(root, "testViewMode", Q_ARG(QVariant, arg));
    return QStringLiteral("OK");
  }
  if (verb == QStringLiteral("insert")) {
    QMetaObject::invokeMethod(root, "testInsert", Q_ARG(QVariant, arg));
    return QStringLiteral("OK");
  }
  if (verb == QStringLiteral("text")) {
    QVariant ret;
    QMetaObject::invokeMethod(root, "testNoteText", Qt::DirectConnection, Q_RETURN_ARG(QVariant, ret));
    return QStringLiteral("OK ") + ret.toString().toUtf8().toBase64();
  }
  if (verb == QStringLiteral("shot")) {
    if (arg.isEmpty()) return QStringLiteral("ERR shot needs path");
    const QImage img = m_quick->grabFramebuffer();
    if (img.isNull() || !img.save(arg))
      return QStringLiteral("ERR save failed");
    return QStringLiteral("OK %1x%2").arg(img.width()).arg(img.height());
  }
  if (verb == QStringLiteral("wait")) {
    QEventLoop loop;
    QTimer::singleShot(arg.toInt(), &loop, &QEventLoop::quit);
    loop.exec();
    return QStringLiteral("OK");
  }
  if (verb == QStringLiteral("quit")) {
    QTimer::singleShot(0, qApp, &QApplication::quit);
    return QStringLiteral("OK");
  }
  return QStringLiteral("ERR unknown: ") + verb;
}
