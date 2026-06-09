#include "globalhotkey.h"

#include <QGuiApplication>

#include <core/hotkey/hotkeyparser.h>

#if __has_include(<xcb/xcb.h>)
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#define MARKLY_HAVE_XCB 1
#endif

using namespace markly;

#ifdef MARKLY_HAVE_XCB
namespace {
xcb_connection_t *xcbConnection() {
  if (auto *x11 = qApp->nativeInterface<QNativeInterface::QX11Application>()) {
    return x11->connection();
  }
  return nullptr;
}

// Map Qt keyboard modifiers to XCB modifier mask.
quint32 toXcbMods(int qtMods) {
  quint32 m = 0;
  if (qtMods & Qt::ShiftModifier) m |= XCB_MOD_MASK_SHIFT;
  if (qtMods & Qt::ControlModifier) m |= XCB_MOD_MASK_CONTROL;
  if (qtMods & Qt::AltModifier) m |= XCB_MOD_MASK_1;
  if (qtMods & Qt::MetaModifier) m |= XCB_MOD_MASK_4;
  return m;
}

// Approximate keysym for a Qt::Key (letters use the lowercase Latin keysym).
xcb_keysym_t toKeysym(int qtKey) {
  if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
    return static_cast<xcb_keysym_t>(qtKey + 0x20); // 'A'(0x41) -> 'a'(0x61)
  }
  return static_cast<xcb_keysym_t>(qtKey); // digits/symbols map directly enough
}

// Lock-modifier combinations to also grab (Caps/Num) so the hotkey fires regardless.
const quint32 c_lockVariants[] = {0, XCB_MOD_MASK_LOCK, XCB_MOD_MASK_2,
                                  XCB_MOD_MASK_LOCK | XCB_MOD_MASK_2};
} // namespace
#endif

GlobalHotkey::GlobalHotkey(QObject *p_parent) : QObject(p_parent) {}

GlobalHotkey::~GlobalHotkey() { unregister(); }

bool GlobalHotkey::registerHotkey(const QString &p_seq) {
  unregister();
  if (QGuiApplication::platformName() != QLatin1String("xcb")) {
    return false; // Wayland / offscreen: portable fallback is the single-instance raise
  }
  const ParsedHotkey hk = HotkeyParser::parse(p_seq);
  if (!hk.valid) {
    return false;
  }
#ifdef MARKLY_HAVE_XCB
  xcb_connection_t *conn = xcbConnection();
  if (!conn) {
    return false;
  }
  xcb_key_symbols_t *syms = xcb_key_symbols_alloc(conn);
  if (!syms) {
    return false;
  }
  xcb_keycode_t *codes = xcb_key_symbols_get_keycode(syms, toKeysym(hk.key));
  if (!codes || codes[0] == XCB_NO_SYMBOL) {
    free(codes);
    xcb_key_symbols_free(syms);
    return false;
  }
  m_keycode = codes[0];
  m_modifiers = toXcbMods(hk.modifiers);
  free(codes);
  xcb_key_symbols_free(syms);

  xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
  if (!screen) {
    return false;
  }
  for (quint32 lock : c_lockVariants) {
    xcb_grab_key(conn, 1, screen->root, m_modifiers | lock, m_keycode,
                 XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
  }
  xcb_flush(conn);
  qApp->installNativeEventFilter(this);
  m_registered = true;
  return true;
#else
  return false;
#endif
}

void GlobalHotkey::unregister() {
#ifdef MARKLY_HAVE_XCB
  if (m_registered) {
    if (xcb_connection_t *conn = xcbConnection()) {
      xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
      if (screen) {
        for (quint32 lock : c_lockVariants) {
          xcb_ungrab_key(conn, m_keycode, screen->root, m_modifiers | lock);
        }
        xcb_flush(conn);
      }
    }
    qApp->removeNativeEventFilter(this);
  }
#endif
  m_registered = false;
}

bool GlobalHotkey::nativeEventFilter(const QByteArray &p_eventType, void *p_message,
                                     qintptr *p_result) {
  Q_UNUSED(p_result);
#ifdef MARKLY_HAVE_XCB
  if (m_registered && p_eventType == QByteArrayLiteral("xcb_generic_event_t")) {
    auto *ev = static_cast<xcb_generic_event_t *>(p_message);
    if ((ev->response_type & ~0x80) == XCB_KEY_PRESS) {
      auto *kp = reinterpret_cast<xcb_key_press_event_t *>(ev);
      const quint32 cleanState =
          kp->state & ~(XCB_MOD_MASK_LOCK | XCB_MOD_MASK_2);
      if (kp->detail == m_keycode && cleanState == m_modifiers) {
        emit activated();
      }
    }
  }
#else
  Q_UNUSED(p_eventType);
  Q_UNUSED(p_message);
#endif
  return false;
}
