#include "mainwindow.h"
#include "customtextitem.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>


// Конструктор главного окна
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Nikitosmos"); // Меняем заголовок окна

    // Создаем бесконечную сцену
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setSceneRect(-5000, -5000, 10000, 10000); // Размеры виртуальной сцены

    ui->graphView->setScene(scene); // Привязываем сцену
    ui->graphView->centerOn(0, 0); // Центрируем координатную сетку
    ui->graphView->scale(25, 25); // Зум до "радиуса" 10

    functionInputs = {ui->func_1}; // Инициализируем вектор полей ввода

    // Привязка начального поля ввоода к обработчику
    connect(ui->func_1, &QLineEdit::editingFinished, this, [this]() {
        this->onFunctionChanged(0, ui->func_1->text());
    }); // Передаем индекс поля и новый текст
}


// Деструктор, освобождаем ui
MainWindow::~MainWindow()
{
    delete ui;
}


// Единый обработчик изменений для любого поля ввода
void MainWindow::onFunctionChanged(int index, const QString &expression)
{
    qDebug() << "Изменилась функция №" << index << ":" << expression;
}


// Обработка нажатия на кнопку добавления поля ввода
void MainWindow::on_addFunc_clicked()
{
    // Проверяем достижение лимита
    if (functionInputs.size() >= MAX_INPUTS) {
        QMessageBox::warning(
            this,
            "Reached limit",
            QString("Reached maximum number of inputs: %1").arg(MAX_INPUTS)
            );
        return;
    }

    // Создаем новое поле ввода
    QLineEdit *newInput = new QLineEdit(this); // Текстовый ввод
    ColorSelector *newColor = new ColorSelector(this); // Соответствующий виджет изменения цвета
    newInput->setPlaceholderText(QString("f%1(x)").arg(functionInputs.size() + 1));

    // Добавляем в вектор
    int newIndex = functionInputs.size();
    functionInputs.append(newInput);

    // Вставляем в лайаут для полей ввода на последнее место
    ui->InputLt->addRow(newInput, newColor);

    // Подключаем обработчик на сигнал окончания редактирования
    connect(newInput, &QLineEdit::editingFinished, this, [this, newIndex]() {
        onFunctionChanged(newIndex, this->functionInputs[newIndex]->text());
    });
}


// Обработка Save as JSON
void MainWindow::on_saveJSON_triggered()
{
    // Путь по умолчанию - Загрузки/save.json
    QString defaultPath = QDir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).filePath("save.json");
    // Диалог выбора имени файла для сохранения
    QString fileName = QFileDialog::getSaveFileName(
        this, // родительский виджет
        "Save as", // заголовок
        defaultPath, // начальная директория
        "json file (*.json)" // фильтр файлов
        );

    if (fileName.isEmpty()) {
        // Отмена, если имя файла не выбрано
        return;
    }
    qDebug() << "Выбранный путь:" << fileName;
    // Запуск экспорта
    buildJSON(fileName);
}


// Экспорт натроек в JSON
void MainWindow::buildJSON(QString savePath)
{
    QJsonObject root; // Корневой объект
    QJsonArray functArr; // Массив функций
    QJsonArray captionsArr; // Массив подписей

    // Проходим по всем строкам лайаута с полями ввода
    for (int row = 0; row < ui->InputLt->rowCount(); ++row) {
        QLayoutItem *labelItem = ui->InputLt->itemAt(row, QFormLayout::LabelRole); // Виджет слева
        QLayoutItem *fieldItem = ui->InputLt->itemAt(row, QFormLayout::FieldRole); // Виджет справа

        if (!labelItem || !fieldItem) continue; // Обработка ошибок

        auto *input = qobject_cast<QLineEdit*>(labelItem->widget()); // Приводим левый виджет к QLineEdit
        auto *selector = qobject_cast<ColorSelector*>(fieldItem->widget()); // Приводим левый виджет к ColorSelector

        if (input && selector) {
            // Для каждой функции сохраняем текст формулы и выбранный цвет графика
            /* Пример:
                {
                    "color": "#e6194b",
                    "expr": "234324"
                }
             */

            QString expr = input->text().trimmed(); // Получаем текст и обрезаем пробелы с краев
            if (expr.isEmpty()) continue; // Пропускаем пустые поля

            QJsonObject func;

            func["expr"] = expr;
            func["color"] = selector->color().name();
            functArr.append(func);
        }
    }

    root["functions"] = functArr; // Добавляем массив функций в корень

    const auto textList = ui->graphView->scene()->items(); // Список всех объектов на сцене
    for (QGraphicsItem *item : std::as_const(textList)) {
        // dynamic_cast вернет nullptr, если объект не нужного типа
        if (auto textItem = dynamic_cast<CustomTextItem*>(item)) {
            // Для каждого TextItem сохраняем текст подписи, координаты, цвет и размер шрифта
            /* Пример:
                {
                    "font": {
                        "color": "#55ffff",
                        "size": 50
                    },
                    "pos": {
                        "x": -1,
                        "y": 1
                    },
                    "text": "QWERTY"
                }
             */
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

    root["captions"] = captionsArr; // Добавляем массив подписей в корень

    QJsonDocument doc(root); // Финальный документ

    // Запись сформированного документа в выбранный файл
    QFile file(savePath);
    if (file.open(QIODevice::WriteOnly)) {
        // QJsonDocument::Indented - красивое форматирование
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "JSON успешно сохранен!";
    } else {
        qWarning() << "Не удалось открыть файл для записи:" << savePath;
    }
}


// Обработка Open From JSON
void MainWindow::on_importJSON_triggered()
{
    // Вывод предупреждения о перезаписи настроек
    QMessageBox::StandardButton reply = QMessageBox::warning(
        this,
        "Предупреждение",
        "Это удалит текущие изменения. Вы действительно хотите продолжить?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel // Кнопка по умолчанию при нажатии Enter
        );

    // Если пользователь нажал Cancel или закрыл окно — прерываем операцию
    if (reply != QMessageBox::Ok) {
        return;
    }

    // Диалог выбора имени файла с сохранением
    QString fileName = QFileDialog::getOpenFileName(
        this, // родительский виджет
        "Open", // заголовок
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), // директория по умолчанию - Загрузки
        "json file (*.json)" // ильтр файлов
        );

    if (fileName.isEmpty()) {
        // Пользователь отменил выбор
        return;
    }
    // Запуск импорта
    importData(fileName);
}


// Импорт настроек из JSON
void MainWindow::importData(QString filePath)
{
    QFile file(filePath);
    // Обработка ошибки открытия файла
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл для чтения";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    // Парсим JSON из байтового массива
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    // Обработка ошибки парсинга
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

        // Заполняем данными
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

        // Извлекаем  массив подписей
        QJsonArray captionsArr = rootObject["captions"].toArray();

        // Удаляем старые подписи
        const auto textList = ui->graphView->scene()->items();
        for (QGraphicsItem *item : std::as_const(textList)) {
            if (auto textItem = dynamic_cast<CustomTextItem*>(item)) {
                delete textItem;
            }
        }

        // Создаем новые
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


// Заглушка для экспорта в PNG
void MainWindow::on_savePNG_triggered()
{
    QMessageBox::warning(
        this,
        "Not supported",
        QString("PNG export temporary not supported. Use screenshot instead")
        );
    return;
}
