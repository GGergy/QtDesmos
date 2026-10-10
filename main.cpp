#include "mainwindow.h"

#include <QApplication>
#include <QIcon>


int main(int argc, char *argv[])
{
    // Корень
    QApplication a(argc, argv);

    // Загрузка иконки из ресурсов
    QIcon appIcon(":/resources/app_icon.ico");
    if (appIcon.isNull()) {
        qDebug() << "Иконка НЕ найдена в ресурсах!";
    } else {
        qDebug() << "Иконка успешно загружена!";
    }
    a.setWindowIcon(appIcon);

    MainWindow w; // Создаем главное окно
    w.show();
    return QApplication::exec(); // Запускаем жизненный цикл приложения
}
