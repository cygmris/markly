#include "localebridge.h"

#include <QQmlEngine>

#include <core/configmgr.h>
#include <core/marklyapp.h>
#include <core/widgetconfig.h>

using namespace markly;

LocaleBridge::LocaleBridge(QQmlEngine *p_engine, QObject *p_parent)
    : QObject(p_parent), m_engine(p_engine) {}

QString LocaleBridge::language() const {
  return ConfigMgr::getInst().getWidgetConfig().getLanguage();
}

void LocaleBridge::setLanguage(const QString &p_language) {
  ConfigMgr::getInst().getWidgetConfig().setLanguage(p_language);
  MarklyApp::getInst().applyLanguage(p_language);
  if (m_engine) {
    m_engine->retranslate(); // qsTr() bindings re-evaluate
  }
  emit changed();
}
