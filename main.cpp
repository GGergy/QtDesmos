#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon appIcon(":/resources/app_icon.ico");
    if (appIcon.isNull()) {
        qDebug() << "Иконка НЕ найдена в ресурсах!";
    } else {
        qDebug() << "Иконка успешно загружена!";
    }
    a.setWindowIcon(appIcon);
    MainWindow w;
    w.show();
    return QApplication::exec();
}
