#ifndef COLORSELECTOR_H
#define COLORSELECTOR_H

#include <QToolButton>

class ColorSelector : public QToolButton {
    Q_OBJECT
public:
    explicit ColorSelector(QWidget *parent = nullptr);
    void setColor(const QColor &color);
    const QColor &color() const;
    void setdefault();

signals:
    void colorChanged(const QColor &color);

private slots:
    void changeColor();

private:
    QColor m_color;
    static const QList<QColor> palette;
    int m_index;
};
#endif // COLORSELECTOR_H
