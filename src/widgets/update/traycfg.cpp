#include "traycfg.h"

#include <core/configmgr.h>
#include <core/widgetconfig.h>

using namespace markly;

TrayCfgQml::TrayCfgQml(QObject *p_parent) : QObject(p_parent) {}

bool TrayCfgQml::minimizeToTray() const {
  return ConfigMgr::getInst().getWidgetConfig().getMinimizeToTray();
}

void TrayCfgQml::setMinimizeToTray(bool p_v) {
  if (minimizeToTray() == p_v) {
    return;
  }
  ConfigMgr::getInst().getWidgetConfig().setMinimizeToTray(p_v);
  emit changed();
}

bool TrayCfgQml::autoUpdateCheck() const {
  return ConfigMgr::getInst().getWidgetConfig().getAutoUpdateCheck();
}

void TrayCfgQml::setAutoUpdateCheck(bool p_v) {
  if (autoUpdateCheck() == p_v) {
    return;
  }
  ConfigMgr::getInst().getWidgetConfig().setAutoUpdateCheck(p_v);
  emit changed();
}

QString TrayCfgQml::globalHotkey() const {
  return ConfigMgr::getInst().getWidgetConfig().getGlobalHotkey();
}

void TrayCfgQml::setGlobalHotkey(const QString &p_v) {
  if (globalHotkey() == p_v) {
    return;
  }
  ConfigMgr::getInst().getWidgetConfig().setGlobalHotkey(p_v);
  emit changed();
}
