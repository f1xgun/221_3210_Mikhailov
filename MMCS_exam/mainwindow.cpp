#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "promocodewidget.h"
#include <QRandomGenerator>
#include "Promocode.h"

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
        generate_promocode(i);
    }
}

void MainWindow::generate_promocode(int index) {
    const auto promocode = new Promocode(generate_random_string(4));
    auto promocodeUi = new PromocodeWidget();
    this->invisiblePromocodeIndexes.push_back(index);
    int row = index / 2;
    int col = index % 2;
    promocodes_grid->addWidget(promocodeUi, row, col);
    this->promocodes.push_back(promocode);
}

QString MainWindow::generate_random_string(int length = 4) {
    const QString possibleCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    QString randomString;

    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        randomString.append(possibleCharacters.at(index));
    }

    return randomString;
}

void MainWindow::on_pushButton_clicked()
{
    if (invisiblePromocodeIndexes.empty()) {
        qWarning() << "No invisible promocodes available.";
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(invisiblePromocodeIndexes.size());
    int promocodeIndex = invisiblePromocodeIndexes.at(randomIndex);
    PromocodeWidget* promocodeWidget = qobject_cast<PromocodeWidget*>(promocodes_grid->itemAt(promocodeIndex)->widget());
    promocodeWidget->setPromocodeText(promocodes.at(promocodeIndex)->code);
    invisiblePromocodeIndexes.erase(std::next(invisiblePromocodeIndexes.begin(), randomIndex), std::next(invisiblePromocodeIndexes.begin(), randomIndex + 1));
    generate_promocode(this->promocodes.size());
}

