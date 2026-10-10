#include "colorselector.h"

#include <QColorDialog>


const QList<QColor> ColorSelector::palette = {
    QColor::fromRgb(0xE6, 0x19, 0x4B), // 1. Ярко-красный
    QColor::fromRgb(0x3C, 0xB4, 0x4B), // 2. Сочный зеленый
    QColor::fromRgb(0x00, 0x82, 0xC8), // 3. Насыщенный синий
    QColor::fromRgb(0xF5, 0x82, 0x31), // 4. Оранжевый
    QColor::fromRgb(0x91, 0x1E, 0xB4), // 5. Фиолетовый
    QColor::fromRgb(0x46, 0xF0, 0xF0), // 6. Голубой / Циан
    QColor::fromRgb(0xF0, 0x32, 0xE6), // 7. Пурпурный / Маджента
    QColor::fromRgb(0xD2, 0xF5, 0x3C), // 8. Лаймовый
    QColor::fromRgb(0xFA, 0xBE, 0xBE), // 9. Розовый
    QColor::fromRgb(0x00, 0x80, 0x80), // 10. Тёмно-бирюзовый (Teal)
    QColor::fromRgb(0xC3, 0xB0, 0x91), // 11. Кремовый хаки
    QColor::fromRgb(0x80, 0x00, 0x00), // 12. Бордовый
    QColor::fromRgb(0x80, 0x80, 0x00), // 13. Оливковый
    QColor::fromRgb(0x00, 0x00, 0x80),  // 14. Тёмно-синий (Navy)
    QColor::fromRgb(0x8A, 0x7F, 0x8E)  // 15. Бледный пурпурно-синий
};


ColorSelector::ColorSelector(QWidget *parent) : QToolButton(parent) {
    static int colorIndex = 0;
    setColor(palette[colorIndex % palette.size()]);
    m_index = colorIndex++;

    connect(this, &QToolButton::clicked, this, &ColorSelector::changeColor);
}


void ColorSelector::setColor(const QColor &color) {
    if (m_color != color) {
        m_color = color;
        setStyleSheet(QString("background-color: %1").arg(m_color.name()));
        emit colorChanged(m_color);
    }
}


const QColor& ColorSelector::color() const {
    return m_color;
}


void ColorSelector::changeColor() {
    QColor newColor = QColorDialog::getColor(m_color, parentWidget());
    if (newColor.isValid()) {
        setColor(newColor);
    }
}


void ColorSelector::setdefault(){
    setColor(palette[m_index % palette.size()]);
}
