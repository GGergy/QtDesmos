#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include "customtextitem.h"

#include <QGraphicsView>
#include <QWheelEvent>
#include <QPointer>
#include <QGraphicsTextItem>
#include <QList>

class GraphView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphView(QWidget *parent = nullptr);

    CustomTextItem *addCaption(QString text, QPointF pos);

protected:
    // Отрисовка координатной сетки и осей
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    // Поддержка зума колесиком мыши
    void wheelEvent(QWheelEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void showTextContextMenu(CustomTextItem *textItem, const QPoint &globalPos);
    void addNewTextDialog(const QPointF &scenePos);

private:
    double m_gridStep = 50.0; // Базовый шаг сетки в пикселях
    const double minScaleThreshold = 1.5;
};

#endif // GRAPHVIEW_H
