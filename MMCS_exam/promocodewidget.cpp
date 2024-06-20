#include "promocodewidget.h"
#include "ui_promocodewidget.h"

PromocodeWidget::PromocodeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PromocodeWidget)
{
    ui->setupUi(this);
    this->setFixedSize(200, 100);
    this->setStyleSheet("border: 1px solid black;");
}

PromocodeWidget::~PromocodeWidget()
{
    delete ui;
}

void PromocodeWidget::setPromocodeText(const QString &text) {
    ui->code->setText(text);
}
