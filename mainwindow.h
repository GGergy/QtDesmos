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

private:
    Ui::MainWindow *ui;
    QVector<QLineEdit*> functionInputs;
    static constexpr int MAX_INPUTS = 14;

    void onFunctionChanged(int index, const QString &expression);
};
#endif // MAINWINDOW_H
