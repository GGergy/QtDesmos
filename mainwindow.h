#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:

    void on_addFunc_clicked();

    void on_saveJSON_triggered();

    void on_importJSON_triggered();

    void on_savePNG_triggered();

private:
    Ui::MainWindow *ui;
    QVector<QLineEdit*> functionInputs;
    static constexpr int MAX_INPUTS = 15;

    void onFunctionChanged(int index, const QString &expression);
    void buildJSON(QString savePath);
    void extracted(QJsonArray &functionsArray);
    void importData(QString filePath);
};
#endif // MAINWINDOW_H
