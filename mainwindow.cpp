#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVector>
#include <QMessageBox>
#include <QGraphicsScene>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Создаем бесконечную сцену
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setSceneRect(-5000, -5000, 10000, 10000); // Размеры виртуальной сцены

    ui->graphView->setScene(scene);
    ui->graphView->centerOn(0, 0); // Центрируем координатную сетку

    functionInputs = {ui->func_1};

    connect(ui->func_1, &QLineEdit::editingFinished, this, [this]() {
        this->onFunctionChanged(0, ui->func_1->text());
    }); // Передаем индекс поля и новый текст
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Единый обработчик изменений для любого поля ввода
void MainWindow::onFunctionChanged(int index, const QString &expression) {
    qDebug() << "Изменилась функция №" << index << ":" << expression;
    // Логика обновления графика № index в вашем QGraphicsView
}

void MainWindow::on_addFunc_clicked() {

    // 0. Проверяем достижение лимита
    if (functionInputs.size() >= MAX_INPUTS) {
        QMessageBox::warning(
            this,
            "Превышен лимит",
            QString("Reached maximum number of inputs: %1").arg(MAX_INPUTS)
            );
        return; // Прерываем создание нового поля
    }
    // 1. Создаем новое поле ввода
    QLineEdit *newInput = new QLineEdit(this);
    newInput->setPlaceholderText(QString("f%1(x)").arg(functionInputs.size() + 1));

    // 2. Добавляем в вектор
    int newIndex = functionInputs.size();
    functionInputs.append(newInput);

    // 3. Вставляем в ваш QVBoxLayout левой панели (например, ui->verticalLayout_Inputs)
    // Вставляем перед последним элементом (Spacer'ом)
    int layoutIndex = ui->FuncLt->count() - 2;
    ui->FuncLt->insertWidget(layoutIndex, newInput);

    // 4. Подключаем сигнал
    connect(newInput, &QLineEdit::editingFinished, this, [this, newIndex]() {
        onFunctionChanged(newIndex, this->functionInputs[newIndex]->text());
    });
}

