#include "customtextitem.h"
#include <QPainter>
#include <QFontMetricsF>
#include <QTextDocument>
#include <QStyleOptionGraphicsItem>
#include <QPen>


CustomTextItem::CustomTextItem(const QString &text, QGraphicsItem *parent)
    : QGraphicsTextItem(text, parent)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

    setFontSize(10);
    setTextColor(Qt::black);

    // Первоначальный расчет сдвига матрицы
    updateCenterTransform();
}

void CustomTextItem::setText(const QString &text)
{
    setPlainText(text);
    updateCenterTransform(); // Обновляем центрирование при изменении текста
}

void CustomTextItem::updateCenterTransform()
{
    // Рассчитываем сдвиг матрицы в пикселях экранных координат
    QRectF rect = boundingRect();
    setTransform(QTransform::fromTranslate(-rect.width() / 2.0, -rect.height() / 2.0));
}

QPainterPath CustomTextItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void CustomTextItem::setTextColor(const QColor &color)
{
    m_textColor = color;
    setDefaultTextColor(color);
    update();
}

QColor CustomTextItem::textColor() const
{
    return m_textColor;
}

void CustomTextItem::setFontSize(int pointSize)
{
    QFont currentFont = font();
    currentFont.setPointSize(pointSize);
    setFont(currentFont);

    // Изменение размера шрифта меняет boundingRect, поэтому пересчитываем центр
    updateCenterTransform();
}

int CustomTextItem::fontSize() const
{
    return font().pointSize();
}

void CustomTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = true;
    update();
    QGraphicsTextItem::hoverEnterEvent(event);
}

void CustomTextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = false;
    update();
    QGraphicsTextItem::hoverLeaveEvent(event);
}

void CustomTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 1. Отрисовка подсветки при наведении
    if (m_isHovered) {
        painter->save();
        painter->setPen(QPen(m_hoverBorderColor, 1, Qt::DashLine));
        painter->setBrush(QColor(0, 120, 215, 20));
        painter->drawRect(boundingRect().adjusted(0.5, 0.5, -0.5, -0.5));
        painter->restore();
    }

    QGraphicsTextItem::paint(painter, option, widget);
}