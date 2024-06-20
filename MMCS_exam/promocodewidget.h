#ifndef PROMOCODEWIDGET_H
#define PROMOCODEWIDGET_H

#include <QWidget>

namespace Ui {
class PromocodeWidget;
}

class PromocodeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PromocodeWidget(QWidget *parent = nullptr);
    ~PromocodeWidget();
    void setPromocodeText(const QString &text);

private:
    Ui::PromocodeWidget *ui;
};

#endif // PROMOCODEWIDGET_H
