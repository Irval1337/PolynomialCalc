#include "pointvalue.h"
#include "ui_pointvalue.h"
#include "qpushbutton.h"
#include <QRegularExpression>
#include <QMessageBox>

pointvalue::pointvalue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pointvalue)
{
    ui->setupUi(this);
}

pointvalue::pointvalue(QWidget *parent, std::vector<QChar> args, std::vector<double>* val) :
    QDialog(parent),
    ui(new Ui::pointvalue)
{
    ui->setupUi(this);
    this->_val = val;
    this->_args = args;
    if (_args.size() == 0) close();

    ui->label->setText(tr("Введите значение переменной ") + _args[_p] + tr(":"));
}

pointvalue::~pointvalue()
{
    delete ui;
}


void pointvalue::on_pushButton_2_clicked()
{
    close();
}

void pointvalue::on_buttonBox_accepted() {

}

void pointvalue::on_pushButton_clicked()
{
    QRegularExpression rx("^(-?)(0|([1-9][0-9]*))(\.[0-9]+)?$");
    if (rx.match(ui->lineEdit->text()).hasMatch()) {
        _val->push_back(ui->lineEdit->text().toDouble());
        _p++;
        if (_p == _args.size()) close();
        ui->label->setText(tr("Введите значение переменной ") + _args[_p] + tr(":"));
        ui->lineEdit->clear();
    } else {
        QMessageBox::warning(this, "PolynomialCalc", "Введенне значение не является числом.");
    }
}

