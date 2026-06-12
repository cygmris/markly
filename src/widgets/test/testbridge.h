#ifndef TESTBRIDGE_H
#define TESTBRIDGE_H

#include <QObject>
#include <QString>

class QQuickWidget;
class QLocalServer;
class QLocalSocket;

namespace markly {

// Offscreen automation bridge (#offscreen-test-bridge). Enabled only when env
// MARKLY_TEST_SOCKET is set. Listens on a local socket and drives the app via the
// QML root's test* helpers + grabFramebuffer screenshots — no input simulation,
// no focus, works fully headless (QT_QPA_PLATFORM=offscreen).
class TestBridge : public QObject {
  Q_OBJECT
public:
  explicit TestBridge(QQuickWidget *p_quick, QObject *p_parent = nullptr);
  bool start(const QString &p_socketPath);

private slots:
  void onNewConnection();
  void onReadyRead();

private:
  QString handle(const QString &p_line);

  QQuickWidget *m_quick = nullptr;
  QLocalServer *m_server = nullptr;
  QLocalSocket *m_client = nullptr;
};

} // namespace markly

#endif // TESTBRIDGE_H
