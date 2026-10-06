#include "colorselector.h"
#include <QWidget>
#include <QColorDialog>
#include <QRandomGenerator>



ColorSelector::ColorSelector(QWidget *parent) : QToolButton(parent) {
    setColor({QRandomGenerator::global()->bounded(0, 256), QRandomGenerator::global()->bounded(0, 256), QRandomGenerator::global()->bounded(0, 256)});
    connect(this, &QToolButton::clicked, this, &ColorSelector::changeColor);
}

void ColorSelector::setColor(const QColor &color) {
    m_color = color;
    setStyleSheet(QString("background-color: %1").arg(m_color.name()));
}

const QColor& ColorSelector::color() const {
    return m_color;
}

void ColorSelector::changeColor() {
    m_color = QColorDialog::getColor(m_color, parentWidget());
    setColor(m_color);
}