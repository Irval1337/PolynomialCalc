#ifndef DERIVATIVE_H
#define DERIVATIVE_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class derivative;
}

class derivative : public QDialog
{
    Q_OBJECT

public:
    derivative(QWidget *parent = nullptr);
    derivative(QWidget *parent, int* val, int* num);
    ~derivative();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::derivative *ui;
    int* _val;
    int* _num;
};

#endif // DERIVATIVE_H
