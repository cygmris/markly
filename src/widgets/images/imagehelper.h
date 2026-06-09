#ifndef MARKLY_IMAGEHELPER_H
#define MARKLY_IMAGEHELPER_H

#include <QObject>
#include <QString>

namespace markly {
// Saves pasted / dropped images into a note's image folder (vx_images) and returns
// the relative markdown path. Injected into QML as context property "Images".
// Source lives in markly_core so it can be unit-tested. (Image host upload is #10b.)
class ImageHelper : public QObject {
  Q_OBJECT
public:
  explicit ImageHelper(QObject *p_parent = nullptr);

  Q_INVOKABLE bool clipboardHasImage() const;
  // Save the clipboard image as PNG under <noteDir>/vx_images/. Returns the relative
  // path ("vx_images/img_xxx.png") or "" on failure / no image / no noteDir.
  Q_INVOKABLE QString pasteImage(const QString &p_noteDir);
  // Copy an existing image file (path or file:// url) into <noteDir>/vx_images/.
  // Returns the relative path or "" (non-image / failure).
  Q_INVOKABLE QString importImage(const QString &p_srcPathOrUrl, const QString &p_noteDir);

  // Read a relative/absolute local image (resolved against baseDir) and return a
  // "data:image/...;base64,..." URI for inline preview rendering. "" if not a local
  // image (http/data) or on failure. Avoids QWebEngine file:// cross-origin limits.
  Q_INVOKABLE QString toDataUri(const QString &p_src, const QString &p_baseDir) const;

private:
  static const QString c_imageFolder; // "vx_images"
  static bool isImageExt(const QString &p_ext);
  // Ensure <noteDir>/vx_images exists; return its absolute path ("" on failure).
  QString ensureImageDir(const QString &p_noteDir) const;
  QString uniqueName(const QString &p_ext) const;
};
} // namespace markly

#endif // MARKLY_IMAGEHELPER_H
