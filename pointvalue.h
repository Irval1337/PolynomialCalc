#ifndef POINTVALUE_H
#define POINTVALUE_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class pointvalue;
}

class pointvalue : public QDialog
{
    Q_OBJECT

public:
    pointvalue(QWidget *parent = nullptr);
    pointvalue(QWidget *parent, std::vector<QChar> args, std::vector<double>* val);
    ~pointvalue();

private slots:
    void on_buttonBox_accepted();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::pointvalue *ui;
    std::vector<double>* _val;
    std::vector<QChar> _args;
    int _p = 0;
};

#endif // POINTVALUE_H
