#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "promocodewidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    promocodes_grid(new QGridLayout)
{
    ui->setupUi(this);

    QWidget *containerWidget = new QWidget(this);
    containerWidget->setLayout(promocodes_grid);

    // Добавляем containerWidget в QScrollArea для прокрутки
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(true);

    // Добавляем scrollArea в основной layout
    ui->verticalLayout_2->layout()->addWidget(scrollArea);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_login_button_clicked()
{
    if (ui->password_input->text().toStdString() == RIGHT_PASSWORD) {
        ui->error_text->setText("");
        ui->stackedWidget->setCurrentIndex(1);
        generate_promocodes(10);
    } else {
        ui->error_text->setText("Неверный пароль!");
    }
}


void MainWindow::generate_promocodes(int count) {
    for (int i = 0; i < count; i++) {
        auto promocodeUi = new PromocodeWidget();
        int row = i / 2;
        int col = i % 2;
        promocodes_grid->addWidget(promocodeUi, row, col);
    }
    qDebug() << promocodes_grid->count();
    // promocodes_grid->setRowStretch(promocodes_grid->rowCount(), 1);
}
