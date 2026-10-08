#include "mainwindow.h"
#include "customtextitem.h"
#include "./ui_mainwindow.h"

#include <QVector>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Nikitosmos"); // Меняем заголовок окна

    // Создаем бесконечную сцену
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setSceneRect(-5000, -5000, 10000, 10000); // Размеры виртуальной сцены

    ui->graphView->setScene(scene);
    ui->graphView->centerOn(0, 0); // Центрируем координатную сетку
    ui->graphView->scale(25, 25); // Зум до "радиуса" 10

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
            "Reached limit",
            QString("Reached maximum number of inputs: %1").arg(MAX_INPUTS)
            );
        return; // Прерываем создание нового поля
    }
    // 1. Создаем новое поле ввода
    QLineEdit *newInput = new QLineEdit(this);
    ColorSelector *newColor = new ColorSelector(this);
    newInput->setPlaceholderText(QString("f%1(x)").arg(functionInputs.size() + 1));

    // 2. Добавляем в вектор
    int newIndex = functionInputs.size();
    functionInputs.append(newInput);

    // 3. Вставляем в ваш QVBoxLayout левой панели (например, ui->verticalLayout_Inputs)
    // Вставляем перед последним элементом (Spacer'ом)
    ui->InputLt->addRow(newInput, newColor);


    // 4. Подключаем сигнал
    connect(newInput, &QLineEdit::editingFinished, this, [this, newIndex]() {
        onFunctionChanged(newIndex, this->functionInputs[newIndex]->text());
    });
}


void MainWindow::on_saveJSON_triggered(){
    QString defaultPath = QDir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).filePath("save.json");
    qDebug() << "called json export";
    QString fileName = QFileDialog::getSaveFileName(
        this, // родительский виджет
        "Save as", // заголовок
        defaultPath, // начальная директория
        "json file (*.json)" // фильтр файлов
        );

    if (fileName.isEmpty()) {
        return;
    }
    qDebug() << "Выбранный путь:" << fileName;
    buildJSON(fileName);
}


void MainWindow::buildJSON(QString savePath){
    QJsonObject root;
    QJsonArray functArr;
    for (int row = 0; row < ui->InputLt->rowCount(); ++row) {
        QLayoutItem *labelItem = ui->InputLt->itemAt(row, QFormLayout::LabelRole);
        QLayoutItem *fieldItem = ui->InputLt->itemAt(row, QFormLayout::FieldRole);

        if (!labelItem || !fieldItem) continue;

        auto *input = qobject_cast<QLineEdit*>(labelItem->widget());
        auto *selector = qobject_cast<ColorSelector*>(fieldItem->widget());

        if (input && selector) {
            QString expr = input->text().trimmed();
            if (expr.isEmpty()) continue; // Пропускаем пустые поля

            QJsonObject func;
            func["expr"] = expr;
            func["color"] = selector->color().name();
            functArr.append(func);
        }
    }

    root["functions"] = functArr;

    QJsonArray captionsArr;
    const auto textList = ui->graphView->scene()->items();
    for (QGraphicsItem *item : std::as_const(textList)) {
        if (auto textItem = dynamic_cast<CustomTextItem*>(item)) {
            QJsonObject cap;
            QJsonObject capFont;
            QJsonObject point;
            cap["text"] = textItem->toPlainText();
            point["x"] = textItem->x();
            point["y"] = textItem->y();
            cap["pos"] = point;
            capFont["color"] = textItem->textColor().name();
            capFont["size"] = textItem->fontSize();
            cap["font"] = capFont;
            captionsArr.append(cap);
        }
    }

    root["captions"] = captionsArr;

    QJsonDocument doc(root);

    QFile file(savePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "JSON успешно сохранен!";
    } else {
        qWarning() << "Не удалось открыть файл для записи:" << savePath;
    }
}



void MainWindow::on_importJSON_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, // родительский виджет
        "Open", // заголовок
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), // начальная директория
        "json file (*.json)" // фильтр файлов
        );

    if (fileName.isEmpty()) {
        // Пользователь отменил выбор
        return;
    }
    importData(fileName);
}


void MainWindow::importData(QString filePath){
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл для чтения";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    // Парсим JSON из байтового массива
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Ошибка парсинга JSON:" << error.errorString();
        return;
    }

    // Проверяем, что корень является объектом
    if (doc.isObject()) {
        QJsonObject rootObject = doc.object();


        // Извлекаем массив функций
        QJsonArray functionsArray = rootObject["functions"].toArray();
        while (ui->InputLt->rowCount() < functionsArray.count() && ui->InputLt->rowCount() < MAX_INPUTS) {
            on_addFunc_clicked(); // Добавляем строки, если в UI их меньше, чем в JSON
        }

        // 2. Заполняем данными
        for (int i = 0; i < ui->InputLt->rowCount(); ++i) {
            QLayoutItem *labelItem = ui->InputLt->itemAt(i, QFormLayout::LabelRole);
            QLayoutItem *fieldItem = ui->InputLt->itemAt(i, QFormLayout::FieldRole);

            if (!labelItem || !fieldItem) continue;

            auto *input = qobject_cast<QLineEdit*>(labelItem->widget());
            auto *selector = qobject_cast<ColorSelector*>(fieldItem->widget());

            if (input && selector) {
                if (i < functionsArray.count()) {
                    QJsonObject funcObj = functionsArray[i].toObject();
                    input->setText(funcObj["expr"].toString());
                    selector->setColor(QColor(funcObj["color"].toString()));
                } else {
                    // Если в JSON элементов меньше, чем сейчас строк на экране — очищаем лишние
                    input->clear();
                    selector->setdefault();
                }
            }
        }

        const auto textList = ui->graphView->scene()->items();
        for (QGraphicsItem *item : std::as_const(textList)) {
            if (auto textItem = dynamic_cast<CustomTextItem*>(item)) {
                delete textItem;
            }
        }

        QJsonArray captionsArr = rootObject["captions"].toArray();
        qDebug() << "captions: " << captionsArr.count();
        for (int i = 0; i < captionsArr.count(); ++i){
            QJsonObject caption = captionsArr[i].toObject();
            QJsonObject font = caption["font"].toObject();
            QJsonObject pos = caption["pos"].toObject();
            CustomTextItem *textItem = ui->graphView->addCaption(caption["text"].toString(), QPointF(pos["x"].toDouble(), pos["y"].toDouble()));
            textItem->setTextColor(font["color"].toString());
            textItem->setFontSize(font["size"].toInt());
        }
    }
}

void MainWindow::on_savePNG_triggered() {
    QMessageBox::warning(
        this,
        "Not supported",
        QString("PNG export temporary not supported. Use screenshot instead")
        );
    return;
}
