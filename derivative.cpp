#include "derivative.h"
#include "qpushbutton.h"
#include "ui_derivative.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>

derivative::derivative(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::derivative)
{
    ui->setupUi(this);
}

derivative::derivative(QWidget *parent, int *val, int *num) :
    QDialog(parent),
    ui(new Ui::derivative)
{
    ui->setupUi(this);
    this->_val = val;
    this->_num = num;
    this->ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Отмена");

    QRegularExpression rx("^[a-z]$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    ui->lineEdit->setValidator(validator);

    QRegularExpression rx2("^[0-9]+$");
    QValidator *validator2 = new QRegularExpressionValidator(rx2, this);
    ui->lineEdit_2->setValidator(validator2);
}

derivative::~derivative()
{
    delete ui;
}

void derivative::on_buttonBox_accepted()
{
    if (ui->lineEdit->text().size() != 0 && ui->lineEdit_2->text().size() != 0) {
        *_val = ui->lineEdit->text()[0].toLatin1() - 'a';
        *_num = ui->lineEdit_2->text().toInt();
    }
}

