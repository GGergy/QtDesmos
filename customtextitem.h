#ifndef CUSTOMTEXTITEM_H
#define CUSTOMTEXTITEM_H

#include <QGraphicsTextItem>
#include <QColor>
#include <QPainterPath>
#include <QGraphicsSceneHoverEvent>

class CustomTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    explicit CustomTextItem(const QString &text, QGraphicsItem *parent = nullptr);

    // Изменение текста с автоматическим пересчетом центрирования
    void setText(const QString &text);

    // Геометрия хитбокса (полная прямоугольная область)
    QPainterPath shape() const override;

    // Стилизация
    void setTextColor(const QColor &color);

    QColor textColor() const;
    void setFontSize(int pointSize);
    int fontSize() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    void updateCenterTransform(); // Метод обновления сдвига

    bool m_isHovered = false;
    QColor m_textColor = Qt::black;
    QColor m_hoverBorderColor = QColor(0, 120, 215, 180);
};
#endif // CUSTOMTEXTITEM_H
