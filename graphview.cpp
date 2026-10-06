#include "graphview.h"
#include <QPainter>
#include <QWheelEvent>
#include <QFontMetrics>
#include <cmath>

GraphView::GraphView(QWidget *parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    // --- ДОБАВЬТЕ ЭТИ СТРОКИ ДЛЯ УСТРАНЕНИЯ АРТЕФАКТОВ: ---

    // 1. Полная перерисовка окна при любом сдвиге/масштабе
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // 2. Отключение двойной буферизации фона (если была включена),
    //    чтобы сбросить фантомный кеш
    setCacheMode(QGraphicsView::CacheNone);

    // 3. (Опционально) Включение аппаратного или более плавного оффскрин-рендеринга
    viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    viewport()->setAttribute(Qt::WA_NoSystemBackground);
}

void GraphView::wheelEvent(QWheelEvent *event)
{
    const double zoomFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(zoomFactor, zoomFactor);
    } else {
        scale(1.0 / zoomFactor, 1.0 / zoomFactor);
    }
}

// Вспомогательная функция вычисления удобного шага сетки (как в Desmos)
static void calculateGridStep(double pixelsPerUnit, double &mainStep, double &subStep, int &decimals)
{
    // Целевое расстояние между главными линиями сетки в пикселях
    const double targetPixels = 100.0;
    const double rawStep = targetPixels / pixelsPerUnit;

    // Вычисляем порядок величины (десятичную степень)
    const double exponent = std::floor(std::log10(rawStep));
    const double pow10 = std::pow(10.0, exponent);
    const double normalized = rawStep / pow10;

    // Выбираем шаг из ряда [1, 2, 5, 10]
    if (normalized < 1.5) {
        mainStep = 1.0 * pow10;
        subStep = mainStep / 5.0; // 5 подделений
    } else if (normalized < 3.5) {
        mainStep = 2.0 * pow10;
        subStep = mainStep / 4.0; // 4 подделения
    } else if (normalized < 7.5) {
        mainStep = 5.0 * pow10;
        subStep = mainStep / 5.0; // 5 подделений
    } else {
        mainStep = 10.0 * pow10;
        subStep = mainStep / 5.0;
    }

    // Рассчитываем количество знаков после запятой для вывода
    decimals = (exponent < 0) ? static_cast<int>(std::abs(exponent)) : 0;

    // Если шаг равен 2.5 * 10^k или аналогично, может потребоваться еще один знак
    double testVal = mainStep / pow10;
    if (std::abs(testVal - std::round(testVal)) > 1e-4) {
        decimals += 1;
    }
}

// Форматирование числа с устранением микроошибок double
static QString formatNumber(double value, int decimals)
{
    if (std::abs(value) < 1e-12) {
        return "0";
    }
    QString str = QString::number(value, 'f', decimals);
    // Удаляем лишние концевые нули и точку при наличии
    if (str.contains('.')) {
        while (str.endsWith('0')) str.chop(1);
        if (str.endsWith('.')) str.chop(1);
    }
    return str;
}

void GraphView::drawBackground(QPainter *painter, const QRectF &rect)
{
    // 1. Заливаем фон
    painter->fillRect(rect, QColor(255, 255, 255));

    // Коэффициент масштаба: сколько экранных пикселей в 1 мировой единице
    const double pixelsPerUnit = transform().m11();

    // Вычисляем оптимальный шаг сетки и количество знаков после запятой
    double mainStep = 1.0;
    double subStep = 0.2;
    int decimals = 0;
    calculateGridStep(pixelsPerUnit, mainStep, subStep, decimals);

    // Границы видимой области в мировых координатах
    const double left   = rect.left();
    const double right  = rect.right();
    const double top    = rect.top();    // В Qt coordinate system Y направлен вниз
    const double bottom = rect.bottom();

    // --- Отрисовка вспомогательной сетки (Sub-grid) ---
    QPen subGridPen(QColor(240, 240, 240), 1);
    subGridPen.setCosmetic(true);
    painter->setPen(subGridPen);

    const int subStartX = static_cast<int>(std::floor(left / subStep));
    const int subEndX   = static_cast<int>(std::ceil(right / subStep));
    for (int i = subStartX; i <= subEndX; ++i) {
        double x = i * subStep;
        painter->drawLine(QPointF(x, top), QPointF(x, bottom));
    }

    const int subStartY = static_cast<int>(std::floor(top / subStep));
    const int subEndY   = static_cast<int>(std::ceil(bottom / subStep));
    for (int j = subStartY; j <= subEndY; ++j) {
        double y = j * subStep;
        painter->drawLine(QPointF(left, y), QPointF(right, y));
    }

    // --- Отрисовка основной сетки (Main-grid) ---
    QPen mainGridPen(QColor(215, 215, 215), 1);
    mainGridPen.setCosmetic(true);
    painter->setPen(mainGridPen);

    const int mainStartX = static_cast<int>(std::floor(left / mainStep));
    const int mainEndX   = static_cast<int>(std::ceil(right / mainStep));
    for (int i = mainStartX; i <= mainEndX; ++i) {
        double x = i * mainStep;
        painter->drawLine(QPointF(x, top), QPointF(x, bottom));
    }

    const int mainStartY = static_cast<int>(std::floor(top / mainStep));
    const int mainEndY   = static_cast<int>(std::ceil(bottom / mainStep));
    for (int j = mainStartY; j <= mainEndY; ++j) {
        double y = j * mainStep;
        painter->drawLine(QPointF(left, y), QPointF(right, y));
    }

    // --- Отрисовка главных осей координат X и Y ---
    QPen axisPen(QColor(100, 100, 100), 1.5);
    axisPen.setCosmetic(true);
    painter->setPen(axisPen);

    if (left <= 0 && right >= 0) {
        painter->drawLine(QPointF(0, top), QPointF(0, bottom));
    }
    if (top <= 0 && bottom >= 0) {
        painter->drawLine(QPointF(left, 0), QPointF(right, 0));
    }

    // =========================================================================
    // Отрисовка меток и чисел в ЭКРАННЫХ КООРДИНАТАХ (чтобы избежать искажения шрифтов)
    // =========================================================================
    painter->save();

    // Переключаем QPainter в координаты пикселей виджета
    painter->setWorldTransform(QTransform());

    QFont font("Sans-Serif", 9);
    painter->setFont(font);
    QFontMetrics fm(font);

    // Видимый прямоугольник в экранных пикселях
    QRect viewportRect = viewport()->rect();
    const int viewWidth  = viewportRect.width();
    const int viewHeight = viewportRect.height();

    // Определение положения осей на экране
    QPointF originScreen = mapFromScene(0, 0);

    // Липание осей к краям экрана при выходе из зоны видимости (Sticky Axis)
    const double axisX_ScreenY = std::clamp(originScreen.y(), 15.0, static_cast<double>(viewHeight - 25));
    const double axisY_ScreenX = std::clamp(originScreen.x(), 35.0, static_cast<double>(viewWidth - 15));

    painter->setPen(QColor(80, 80, 80));

    // --- Подписи по оси X ---
    for (int i = mainStartX; i <= mainEndX; ++i) {
        if (i == 0) continue; // Пропускаем центр координат

        double xScene = i * mainStep;
        QPointF screenPt = mapFromScene(xScene, 0);

        if (screenPt.x() < 0 || screenPt.x() > viewWidth) continue;

        // Маленькая засечка
        painter->drawLine(QPointF(screenPt.x(), axisX_ScreenY - 3),
                          QPointF(screenPt.x(), axisX_ScreenY + 3));

        QString text = formatNumber(xScene, decimals);
        int textWidth = fm.horizontalAdvance(text);

        QRectF textRect(screenPt.x() - textWidth / 2.0,
                        axisX_ScreenY + 5,
                        textWidth,
                        fm.height());

        painter->drawText(textRect, Qt::AlignCenter, text);
    }

    // --- Подписи по оси Y ---
    for (int j = mainStartY; j <= mainEndY; ++j) {
        if (j == 0) continue; // Пропускаем центр координат

        double yScene = j * mainStep;
        QPointF screenPt = mapFromScene(0, yScene);

        if (screenPt.y() < 0 || screenPt.y() > viewHeight) continue;

        // Маленькая засечка
        painter->drawLine(QPointF(axisY_ScreenX - 3, screenPt.y()),
                          QPointF(axisY_ScreenX + 3, screenPt.y()));

        // В декартовой системе Y направлен вверх, а в сцене Qt — вниз
        QString text = formatNumber(-yScene, decimals);
        int textWidth = fm.horizontalAdvance(text);

        QRectF textRect(axisY_ScreenX - textWidth - 7,
                        screenPt.y() - fm.height() / 2.0,
                        textWidth,
                        fm.height());

        painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, text);
    }

    // --- Подпись Ноля (0) в центре осей ---
    if (left <= 0 && right >= 0 && top <= 0 && bottom >= 0) {
        QString zeroText = "0";
        int zWidth = fm.horizontalAdvance(zeroText);
        QRectF zeroRect(originScreen.x() - zWidth - 6,
                        originScreen.y() + 4,
                        zWidth,
                        fm.height());
        painter->drawText(zeroRect, Qt::AlignCenter, zeroText);
    }

    painter->restore();
}