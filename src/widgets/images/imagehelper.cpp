#include "imagehelper.h"

#include <QClipboard>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QImage>
#include <QMimeData>
#include <QUrl>

using namespace markly;

const QString ImageHelper::c_imageFolder = QStringLiteral("vx_images");

ImageHelper::ImageHelper(QObject *p_parent) : QObject(p_parent) {}

bool ImageHelper::isImageExt(const QString &p_ext) {
  static const QStringList exts = {QStringLiteral("png"),  QStringLiteral("jpg"),
                                   QStringLiteral("jpeg"), QStringLiteral("gif"),
                                   QStringLiteral("bmp"),  QStringLiteral("webp")};
  return exts.contains(p_ext.toLower());
}

QString ImageHelper::ensureImageDir(const QString &p_noteDir) const {
  if (p_noteDir.isEmpty()) {
    return QString();
  }
  QDir dir(p_noteDir);
  if (!dir.exists()) {
    return QString();
  }
  if (!dir.exists(c_imageFolder) && !dir.mkpath(c_imageFolder)) {
    return QString();
  }
  return dir.absoluteFilePath(c_imageFolder);
}

QString ImageHelper::uniqueName(const QString &p_ext) const {
  return QStringLiteral("img_%1.%2")
      .arg(QDateTime::currentMSecsSinceEpoch())
      .arg(p_ext.toLower());
}

bool ImageHelper::clipboardHasImage() const {
  const auto *mime = QGuiApplication::clipboard()->mimeData();
  return mime && mime->hasImage();
}

QString ImageHelper::pasteImage(const QString &p_noteDir) {
  const QImage img = QGuiApplication::clipboard()->image();
  if (img.isNull()) {
    return QString();
  }
  const auto absDir = ensureImageDir(p_noteDir);
  if (absDir.isEmpty()) {
    return QString();
  }
  const auto name = uniqueName(QStringLiteral("png"));
  if (!img.save(absDir + QLatin1Char('/') + name, "PNG")) {
    return QString();
  }
  return c_imageFolder + QLatin1Char('/') + name;
}

QString ImageHelper::importImage(const QString &p_srcPathOrUrl, const QString &p_noteDir) {
  // Resolve file:// urls to a local path.
  QString srcPath = p_srcPathOrUrl;
  const QUrl url(p_srcPathOrUrl);
  if (url.isLocalFile()) {
    srcPath = url.toLocalFile();
  }
  const QFileInfo srcInfo(srcPath);
  if (!srcInfo.exists() || !srcInfo.isFile() || !isImageExt(srcInfo.suffix())) {
    return QString();
  }
  const auto absDir = ensureImageDir(p_noteDir);
  if (absDir.isEmpty()) {
    return QString();
  }
  const auto name = uniqueName(srcInfo.suffix());
  if (!QFile::copy(srcPath, absDir + QLatin1Char('/') + name)) {
    return QString();
  }
  return c_imageFolder + QLatin1Char('/') + name;
}

QString ImageHelper::toDataUri(const QString &p_src, const QString &p_baseDir) const {
  if (p_src.isEmpty() || p_src.startsWith(QStringLiteral("http://")) ||
      p_src.startsWith(QStringLiteral("https://")) || p_src.startsWith(QStringLiteral("data:"))) {
    return QString();
  }
  QString path = p_src;
  if (p_src.startsWith(QStringLiteral("file://"))) {
    path = QUrl(p_src).toLocalFile();
  } else if (!QDir::isAbsolutePath(p_src)) {
    if (p_baseDir.isEmpty()) {
      return QString();
    }
    path = p_baseDir + QLatin1Char('/') + p_src;
  }
  const QFileInfo info(path);
  if (!info.exists() || !info.isFile() || !isImageExt(info.suffix())) {
    return QString();
  }
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly)) {
    return QString();
  }
  const QByteArray bytes = f.readAll();
  QString mime = info.suffix().toLower();
  if (mime == QStringLiteral("jpg")) {
    mime = QStringLiteral("jpeg");
  } else if (mime == QStringLiteral("svg")) {
    mime = QStringLiteral("svg+xml");
  }
  return QStringLiteral("data:image/%1;base64,%2")
      .arg(mime, QString::fromLatin1(bytes.toBase64()));
}
