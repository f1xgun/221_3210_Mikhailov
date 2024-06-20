#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Promocode.h"
#include "qgridlayout.h"
#include <QMainWindow>
#include <QScrollArea>

const std::string RIGHT_PASSWORD = "123";

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_login_button_clicked();

private:
    Ui::MainWindow *ui;
    QGridLayout *promocodes_grid;
    QScrollArea *scrollArea;
    std::vector<Promocode> promocodes;

    void generate_promocodes(int count);
};
#endif // MAINWINDOW_H
