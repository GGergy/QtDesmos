#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class GraphView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphView(QWidget *parent = nullptr);

protected:
    // Отрисовка координатной сетки и осей
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    // Поддержка зума колесиком мыши
    void wheelEvent(QWheelEvent *event) override;

private:
    double m_gridStep = 50.0; // Базовый шаг сетки в пикселях
};

#endif // GRAPHVIEW_H
