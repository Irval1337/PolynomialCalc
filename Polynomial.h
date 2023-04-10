#include "LinkedList.h"
#include <vector>
#include <QChar>
#include <QString>
#include <Exception.h>
#include <QObject>

struct Element {
    double coefficient;
    std::vector<int> degrees = std::vector<int>(26, 0);
    Element(const Element&);
    Element(double, std::vector<int>);
    bool operator<(Element&);
    bool equals(Element&);
    bool operator>(Element&);
    bool operator==(Element&);
    bool operator!=(Element&);
    Element operator*(Element&);
};

class Polynomial {
private:
    enum StateType {
        num, letter, degree
    };

    DoubleList<Element>* _data;
    std::vector<int> _args;

    double _pow(double, int);
    bool _check_root(double);
    int _index_of_any(QString, QString, int);
    std::vector<int> _get_args(DoubleList<Element>*);
    int _next_state(int, QChar);
    bool _polynomial_checker(QString);
    long long _get_number(QString&, std::size_t&, bool);

public:
    Polynomial(DoubleList<Element>*, std::vector<int>&);
    Polynomial(double);
    Polynomial(QString);
    QString to_string();
    std::vector<QChar> get_args_char();
    double value(std::vector<double>);
    bool is_multiple();
    bool is_zero(bool arg = false);
    Polynomial derivative(int);
    std::pair<Polynomial, Polynomial> div(Polynomial);
    std::vector<double> get_roots();
    Polynomial operator+(Polynomial&);
    Polynomial operator-(Polynomial&);
    Polynomial operator*(Polynomial&);
    bool operator==(Polynomial&);
    bool operator!=(Polynomial&);
};
