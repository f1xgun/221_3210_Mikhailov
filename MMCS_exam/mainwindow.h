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

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QGridLayout *promocodes_grid;
    QScrollArea *scrollArea;
    std::vector<Promocode*> promocodes;
    QString generate_random_string(int length);
    std::vector<int> invisiblePromocodeIndexes;
    QByteArray key;

    void generate_promocodes(int count);
    void generate_promocode(int index);
    int encrypt_code(const QByteArray& key_hex, const QByteArray & plainBytes, QByteArray & encryptedBytes);
    int decrypt_code(const QByteArray& key_hex, const QByteArray & encryptedBytes, QByteArray & decryptedBytes);
};
#endif // MAINWINDOW_H
