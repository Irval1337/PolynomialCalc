#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "derivative.h"
#include "pointvalue.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    _polynomials = new DoubleList<Polynomial>();
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    if (!this->_changed) {
        event->accept();
        return;
    }

    QMessageBox messageBox(QMessageBox::Question,
                tr("PolynomialCalc"),
                tr("В текущем файле есть несохраненные изменения. Вы действительно хотите выйти?"),
                QMessageBox::Default,
                this);
    messageBox.addButton("Да", QMessageBox::YesRole);
    messageBox.addButton("Нет", QMessageBox::NoRole);
    int quit = messageBox.exec();
    if (quit == 0) {
        event->accept();
    } else {
        event->ignore();
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_triggered() // О программе -> Автор
{
    QDesktopServices::openUrl(QUrl("https://irval1337.github.io/"));
}


void MainWindow::on_actionQT_triggered() // О программе -> QT
{
    QDesktopServices::openUrl(QUrl("https://www.qt.io/"));
}


void MainWindow::on_pushButton_clicked()
{
    try {
        QString s = ui->lineEdit->text();
        Polynomial* p = new Polynomial(s);
        this->_polynomials->push_back(p);
        ui->listWidget->addItem(p->to_string());
        ui->lineEdit->clear();
        _changed = true;
    } catch(Exception ex) {
        QMessageBox::critical(this, "PolynomialCalc", tr("При обработке многочлена возникла ошибка: ") + ex.what());
    }
}


void MainWindow::on_action_2_triggered() // Файл -> Новый
{
    _path = "";
    _changed = true;
    setWindowTitle("PolynomialCalc");

}

bool MainWindow::open(QString path) {
    try {
        QFile file(path);
        if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return false;
        }

        DoubleList<Polynomial>* new_data = new DoubleList<Polynomial>();
        QString data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject json = doc.object();

        if (!json.contains("data")) return false;
        QJsonArray actions = json["data"].toArray();
        for (const QJsonValue & val1 : actions) {
            if (!val1.isString()) return false;
            QString s = val1.toString();
            try {
                Polynomial* p = new Polynomial(s);
                new_data->push_back(p);
            } catch (...) {
                continue;
            }
        }

        _polynomials->clear();
        delete _polynomials;
        _polynomials = new_data;
        this->_path = path;
        return true;
    } catch (...) {
        return false;
    }
}

bool MainWindow::save(QString path) {
    if (path == "") {
        return false;
    }

    try {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            return false;
        }

        QJsonObject json;
        QJsonArray data;
        auto p = _polynomials->begin();
        while (p != nullptr) {
            data.push_back(p->data->to_string());
            p = p->next;
        }

        json["data"] = data;
        QTextStream stream(&file);
        stream << QJsonDocument(json).toJson();
        file.flush();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

void MainWindow::on_action_3_triggered() // Файл -> Открыть...
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открытие"), "", tr("Polynomial base (*.pol)"));
    if (!open(fileName)) {
        QMessageBox::critical(this, "PolynomialCalc", "В процессе открытия произошла ошибка. Возможно, файл поврежден.");
    } else {
        _path = fileName;
        _changed = false;
        this->setWindowTitle("PolynomialCalc (" + fileName + ")");
        ui->listWidget->clear();
        auto p = _polynomials->begin();
        while (p != nullptr) {
            ui->listWidget->addItem(p->data->to_string());
            p = p->next;
        }
    }
}


void MainWindow::on_action_4_triggered() // Файл -> Сохранить
{
    if (!save(_path)) {
        QMessageBox::warning(this, "PolynomialCalc", "В процессе сохранения произошла ошибка. Возможно, " \
                                 "вы пытаетесь сохранить файл по пустому пути. В таком случае воспользуйтесь \"Сохранить как...\"");
    } else {
        this->_changed = false;
    }
}


void MainWindow::on_action_5_triggered() // Файл -> Сохранить как...
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранение"), "", tr("Polynomial base (*.pol)"));
    if (save(fileName)) {
        this->setWindowTitle("PolynomialCalc (" + fileName + ")");
        this->_changed = false;
    } else {
        QMessageBox::warning(this, "PolynomialCalc", "В процессе сохранения произошла ошибка. Возможно, " \
                                 "вы пытаетесь сохранить файл по пустому пути.");
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    auto selected = ui->listWidget->selectionModel()->selectedIndexes();
    std::vector<int> inds;
    for(qsizetype i = 0; i < selected.size(); ++i) {
        inds.push_back(selected[i].row());
    }
    std::sort(inds.rbegin(), inds.rend());
    for(std::size_t i = 0; i < inds.size(); ++i) {
        _polynomials->erase(_polynomials->at(inds[i]));
        if (!_changed) _changed = true;
    }
    qDeleteAll(ui->listWidget->selectedItems());
}


void MainWindow::on_pushButton_2_clicked() // Сумма
{
    auto selected = ui->listWidget->selectionModel()->selectedIndexes();
    if (selected.size() <= 1) return;
    std::vector<int> inds;
    Polynomial p(0);
    for(qsizetype i = 0; i < selected.size(); ++i) {
        p = p + *_polynomials->at(selected[i].row())->data;
    }

    QString res = p.to_string();
    QMessageBox messageBox(QMessageBox::Question,
                tr("PolynomialCalc"),
                tr("Результат:\n") + res + tr("\nЖелаете сохранить его в базу?"),
                QMessageBox::Default,
                this);
    messageBox.addButton("Да", QMessageBox::YesRole);
    messageBox.addButton("Нет", QMessageBox::NoRole);
    int quit = messageBox.exec();
    ui->listWidget->clearSelection();
    if (quit == 0) {
        Polynomial* q = new Polynomial(res);
        _polynomials->push_back(q);
        ui->listWidget->addItem(q->to_string());
        this->_changed = true;
    } else {
        return;
    }
}


void MainWindow::on_pushButton_10_clicked() // Произведение
{
    auto selected = ui->listWidget->selectionModel()->selectedIndexes();
    if (selected.size() <= 1) return;
    Polynomial p(1);
    for(qsizetype i = 0; i < selected.size(); ++i) {
        p = p * *_polynomials->at(selected[i].row())->data;
    }

    p = Polynomial(p.to_string());

    QString res = p.to_string();
    QMessageBox messageBox(QMessageBox::Question,
                tr("PolynomialCalc"),
                tr("Результат:\n") + res + tr("\nЖелаете сохранить его в базу?"),
                QMessageBox::Default,
                this);
    messageBox.addButton("Да", QMessageBox::YesRole);
    messageBox.addButton("Нет", QMessageBox::NoRole);
    int quit = messageBox.exec();
    ui->listWidget->clearSelection();
    if (quit == 0) {
        Polynomial* q = new Polynomial(res);
        _polynomials->push_back(q);
        ui->listWidget->addItem(q->to_string());
        this->_changed = true;
    } else {
        return;
    }
}

void MainWindow::on_pushButton_9_clicked() // Равенство
{
    auto selected = ui->listWidget->selectionModel()->selectedIndexes();
    if (selected.size() != 2) {
        QMessageBox::warning(this, "PolynomialCalc", "Для данной операции необходимо выбрать ровно 2 многочлена.");
        ui->listWidget->clearSelection();
        return;
    }

    bool val = *_polynomials->at(selected[0].row())->data == *_polynomials->at(selected[1].row())->data;
    QMessageBox::information(this, "PolynomialCalc", "Результат сравнения:\n" + tr(val ? "Многочлены равны" : "Многочлены не равны"));
    ui->listWidget->clearSelection();
}


void MainWindow::on_pushButton_6_clicked() // Деление
{
    try {
        auto selected = ui->listWidget->selectionModel()->selectedIndexes();
        if (selected.size() != 2) {
            QMessageBox::warning(this, "PolynomialCalc", "Для данной операции необходимо выбрать ровно 2 многочлена.");
            ui->listWidget->clearSelection();
            return;
        }
        auto val = _polynomials->at(selected[0].row())->data->div(*_polynomials->at(selected[1].row())->data);
        Polynomial* p = new Polynomial(val.first), *q = new Polynomial(val.second);

        QMessageBox messageBox(QMessageBox::Question,
                    tr("PolynomialCalc"),
                    tr("Результат операции:\n") + p->to_string() + tr("\nОстаток:\n") + q->to_string() + tr("\nЖелаете сохранить его в базу по возможности?"),
                    QMessageBox::Default,
                    this);
        messageBox.addButton("Да", QMessageBox::YesRole);
        messageBox.addButton("Нет", QMessageBox::NoRole);
        int quit = messageBox.exec();
        if (quit == 0) {
            try {
                Polynomial pp(p->to_string());
                _polynomials->push_back(p);
                ui->listWidget->addItem(p->to_string());
                this->_changed = true;
            } catch (...) {

            }
            try {
                Polynomial qq(q->to_string());
                _polynomials->push_back(q);
                ui->listWidget->addItem(q->to_string());
                this->_changed = true;
            } catch (...) {

            }
        } else {
            delete p;
            delete q;
        }
    } catch (Exception ex) {
        QMessageBox::warning(this, "PolynomialCalc", ex.what());
    }
    ui->listWidget->clearSelection();
}


void MainWindow::on_pushButton_5_clicked() // Найти целые корни
{
    try {
        auto selected = ui->listWidget->selectionModel()->selectedIndexes();
        if (selected.size() != 1) {
            QMessageBox::warning(this, "PolynomialCalc", "Для данной операции необходимо выбрать ровно 1 многочлен.");
            ui->listWidget->clearSelection();
            return;
        }
        QString res = "";
        auto val = _polynomials->at(selected[0].row())->data->get_roots();
        for(std::size_t i = 0; i < val.size(); ++i) {
            res += QString::number(val[i]) + ", ";
        }
        if (res.size() > 0) res = res.left(res.size() - 2);
        QMessageBox::information(this, "PolynomialCalc", tr("Найденные целые корни:\n") + res);
    } catch (Exception ex) {
        QMessageBox::warning(this, "PolynomialCalc", ex.what());
    }
    ui->listWidget->clearSelection();
}


void MainWindow::on_pushButton_7_clicked() // Производная
{
    auto selected = ui->listWidget->selectionModel()->selectedIndexes();
    if (selected.size() != 1) {
        QMessageBox::warning(this, "PolynomialCalc", "Для данной операции необходимо выбрать ровно 1 многочлен.");
        ui->listWidget->clearSelection();
        return;
    }

    int c = -1, cnt = 0;
    derivative* der = new derivative(this, &c, &cnt);
    der->exec();
    if (c == -1) return;
    Polynomial* p = new Polynomial(*_polynomials->at(selected[0].row())->data);
    for(int i = 0; i < cnt; ++i) {
        auto t = p;
        p = new Polynomial(p->derivative(c));
        delete t;
    }
    QString res = p->to_string();
    QMessageBox messageBox(QMessageBox::Question,
                tr("PolynomialCalc"),
                tr("Результат:\n") + res + tr("\nЖелаете сохранить его в базу?"),
                QMessageBox::Default,
                this);
    messageBox.addButton("Да", QMessageBox::YesRole);
    messageBox.addButton("Нет", QMessageBox::NoRole);
    int quit = messageBox.exec();
    ui->listWidget->clearSelection();
    if (quit == 0) {
        Polynomial* q = new Polynomial(res);
        _polynomials->push_back(q);
        ui->listWidget->addItem(q->to_string());
        this->_changed = true;
    } else {
        delete p;
        return;
    }
}


void MainWindow::on_pushButton_8_clicked() // Значение в точке
{
    auto selected = ui->listWidget->selectionModel()->selectedIndexes();
    if (selected.size() != 1) {
        QMessageBox::warning(this, "PolynomialCalc", "Для данной операции необходимо выбрать ровно 1 многочлен.");
        ui->listWidget->clearSelection();
        return;
    }

    if (_polynomials->at(selected[0].row())->data->get_args_char().size() == 0) {
        ui->listWidget->clearSelection();
        QMessageBox::warning(this, "PolynomialCalc", "У многочлена отсутсвуют переменные.");
        return;
    }

    std::vector<double> vals;
    pointvalue* pv = new pointvalue(this, _polynomials->at(selected[0].row())->data->get_args_char(), &vals);
    pv->exec();

    if (vals.size() == 0 || vals.size() < _polynomials->at(selected[0].row())->data->get_args_char().size()) return;
    QMessageBox::information(this, "PolynomialCalc", tr("Значение в указанной точке:\n") + QString::number(_polynomials->at(selected[0].row())->data->value(vals)));
    ui->listWidget->clearSelection();
}

