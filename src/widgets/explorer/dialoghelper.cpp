#include "dialoghelper.h"

#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

using namespace markly;

DialogHelper::DialogHelper(QWidget *p_parent, QObject *p_objParent)
    : QObject(p_objParent), m_parent(p_parent) {}

QString DialogHelper::chooseDirectory(const QString &p_title) {
  return QFileDialog::getExistingDirectory(m_parent, p_title);
}

QString DialogHelper::saveFile(const QString &p_title, const QString &p_defaultName,
                               const QString &p_filter) {
  return QFileDialog::getSaveFileName(m_parent, p_title, p_defaultName, p_filter);
}

QString DialogHelper::promptText(const QString &p_title, const QString &p_label,
                                 const QString &p_default) {
  bool ok = false;
  const auto text =
      QInputDialog::getText(m_parent, p_title, p_label, QLineEdit::Normal, p_default, &ok);
  return ok ? text : QString();
}

QString DialogHelper::chooseColor(const QString &p_initial) {
  const QColor initial = p_initial.isEmpty() ? QColor(Qt::white) : QColor(p_initial);
  const auto color = QColorDialog::getColor(initial, m_parent, QStringLiteral("选择颜色"));
  return color.isValid() ? color.name(QColor::HexRgb) : QString();
}

bool DialogHelper::confirm(const QString &p_title, const QString &p_text) {
  return QMessageBox::question(m_parent, p_title, p_text,
                               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
}

void DialogHelper::notify(const QString &p_title, const QString &p_text) {
  QMessageBox::information(m_parent, p_title, p_text);
}
