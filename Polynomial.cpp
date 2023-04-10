#include "Polynomial.h"

Element::Element(const Element& elem) {
    this->coefficient = elem.coefficient;
    this->degrees = elem.degrees;
}
Element::Element(double coeff, std::vector<int> degs) {
    this->coefficient = coeff;
    this->degrees = degs;
}
bool Element::operator<(Element& sec) {
    for(std::size_t i = 0; i < degrees.size(); ++i) {
        if (degrees[i] < sec.degrees[i]) return true;
        if (degrees[i] > sec.degrees[i]) return false;
    }
    return false;
}
bool Element::equals(Element& sec) {
    return degrees == sec.degrees;
}
bool Element::operator>(Element& sec) {
    for(std::size_t i = 0; i < degrees.size(); ++i) {
        if (degrees[i] > sec.degrees[i]) return true;
        if (degrees[i] < sec.degrees[i]) return false;
    }
    return false;
}
bool Element::operator==(Element& sec) {
    return coefficient == sec.coefficient && degrees == sec.degrees;
}
bool Element::operator!=(Element& sec) {
    return !(*this == sec);
}
Element Element::operator*(Element& sec) {
    std::vector<int> degs = degrees;
    for(std::size_t i = 0; i < degs.size(); ++i) {
        degs[i] += sec.degrees[i];
    }
    return Element(coefficient * sec.coefficient, degs);
}

double Polynomial::_pow(double x, int degree) {
    double res = 1;
    while (degree) {
        if (degree & 1)
            res *= x;
        x *= x;
        degree >>= 1;
    }
    return res;
}
bool Polynomial::_check_root(double x) {
    int t = (int)x;
    if (std::fabs(x - t) > 1e-6) return false;
    return value(std::vector<double>(1, x)) == 0;
}
int Polynomial::_index_of_any(QString s, QString any, int start) {
    for(qsizetype i = start; i < s.size(); ++i) {
        if (any.contains(s[i])) return i;
    }
    return -1;
}
std::vector<int> Polynomial::_get_args(DoubleList<Element>* data) {
    std::vector<int> new_args;
    auto q = data->begin();
    while (q != nullptr) {
        for(std::size_t i = 0; i < q->data->degrees.size(); ++i) {
            if (q->data->degrees[i] == 0) continue;
            new_args.push_back(i);
        }
        q = q->next;
    }
    std::sort(new_args.begin(), new_args.end());
    new_args.erase(std::unique(new_args.begin(), new_args.end()), new_args.end());
    return new_args;
}
int Polynomial::_next_state(int prev, QChar c) {
    switch (prev) {
    case 0:
        if (c == '+' || c == '-') return 1;
        return -1;
    case 1:
        if (c >= '0' && c <= '9') return 1;
        if (c >= 'a' && c <= 'z') return 2;
        if (c == '+' || c == '-') return 1;
        return -1;
    case 2:
        if (c >= 'a' && c <= 'z') return 2;
        if (c == '+' || c == '-') return 1;
        if (c == '^') return 3;
        return -1;
    case 3:
        if (c == '+') return 4;
        return -1;
    case 4:
        if (c >= '0' && c <= '9') return 4;
        if (c == '+' || c == '-') return 1;
        if (c >= 'a' && c <= 'z') return 2;
    default:
        return -1;
    }
}
bool Polynomial::_polynomial_checker(QString s) {
    int state = 0;
    for(qsizetype i = 0; i < s.size(); ++i) {
        state = _next_state(state, s[i]);
        if (state == -1) throw Exception(std::string("Неожиданный символ на позиции ") + std::to_string(i) + '.');
    }
    if (state == 1 || state == 2 || state == 4) return true;
    throw Exception("Неожиданный конец строки.");
    return false;
}
long long Polynomial::_get_number(QString& s, std::size_t& i, bool need_sign) {
    QString snum = "";
    if (s[i] == '+' || s[i] == '-') {
        snum.push_back(s[i]);
        ++i;
    } else if (need_sign) throw Exception("Ожидался символ знака перед числом.");

    while (i < s.size()) {
        if (s[i] < '0' || s[i] > '9') break;
        snum.push_back(s[i]);
        ++i;
    }
    if (snum == "-" || snum == "+") throw Exception("Полученное число состоит только из знака.");
    return snum.toLongLong();
}

Polynomial::Polynomial(DoubleList<Element>* data, std::vector<int>& args) {
    _data = data;
    _args = args;
}
Polynomial::Polynomial(double num) {
    _data = new DoubleList<Element>();
    _data->push_back(new Element(num, std::vector<int>(26, 0)));
    _args = {};
}
Polynomial::Polynomial(QString s) {
    Polynomial p(0);

    QString args = "qwertyuiopasdfghjklzxcvbnm";
    QString deg = "^";
    QString sign = "+-";
    QString digits = "1234567890";

    QString edited = "";
    for(qsizetype i = 0; i < s.size(); ++i) {
        if (s[i] == ' ') continue;
        if (args.contains(s[i]) && (i == 0 || sign.contains(s[i - 1]))) {
            if (i == 0) edited.push_back('+');
            edited.push_back('1');
        }
        if (digits.contains(s[i]) && (i == 0 || s[i - 1] == '^')) {
            edited.push_back('+');
        }
        edited.push_back(s[i]);
    }
    s = edited;

    _polynomial_checker(s);

    std::size_t i = 0;
    StateType state = StateType::num;
    Element el = Element(0, std::vector<int>(26, 0));
    std::vector<int> __args;
    long long num, letter, degree;
    for(; i < s.size(); ) {
        switch (state) {
        case StateType::num: {
            DoubleList<Element>* tmp_list = new DoubleList<Element>();
            tmp_list->push_back(new Element(el));
            Polynomial q(tmp_list, __args);
            p = p + q;
            __args.clear();
            el.degrees = std::vector<int>(26, 0);
            num = _get_number(s, i, true);
            el.coefficient = num;
            state = StateType::letter;
            break;
        }
        case StateType::letter: {
            if (!args.contains(s[i])) {
                state = StateType::num;
                continue;
            }
            letter = s[i].toLatin1() - ((QChar)'a').toLatin1();
            __args.push_back(letter);
            std::sort(__args.begin(), __args.end());
            __args.erase(std::unique(__args.begin(), __args.end()), __args.end());
            el.degrees[letter] += 1;
            i++;
            state = StateType::degree;
            break;
        }
        case StateType::degree: {
            if (s[i] != '^') {
                degree = 1;
            } else {
                i++;
                degree = _get_number(s, i, true);
            }

            el.degrees[letter] += degree - 1;
            if (i < s.size() && args.contains(s[i])) {
                state = StateType::letter;
            } else {
                state = StateType::num;
            }
            break;
        }
        }
    }

    DoubleList<Element>* tmp_list = new DoubleList<Element>();
    tmp_list->push_back(new Element(el));
    Polynomial q(tmp_list, __args);
    p = p + q;
    p._data->sort();
    *this = p;
}
QString Polynomial::to_string() {
    QString ans = "";
    auto p = _data->begin();
    while (p != nullptr) {
        ans += p->data->coefficient >= 0 ? "+" : "-";
        if (fabs(p->data->coefficient) != 1 || p->data->degrees == std::vector<int>(26, 0))
            ans += QString::number(fabs(p->data->coefficient), 'g', 30);
        for(std::size_t i = 0; i < p->data->degrees.size(); ++i) {
            if (p->data->degrees[i] == 0) continue;
            ans += QChar((char)i + 'a');
            if (p->data->degrees[i] != 1)
                ans += '^' + QString::number(p->data->degrees[i]);
        }
        p = p->next;
    }
    if (ans.size() != 0 && ans[0] == '+') ans.remove(0, 1);
    if (ans.size() == 0) ans = "0";
    return ans;
}
std::vector<QChar> Polynomial::get_args_char() {
    std::vector<QChar> args;
    for(std::size_t i = 0; i < _args.size(); ++i) {
        args.push_back(QChar('a' + _args[i]));
    }
    return args;
}
double Polynomial::value(std::vector<double> args) {
    std::vector<double> args_data(26);
    for(std::size_t i = 0; i < args.size(); ++i) {
        args_data[_args[i]] = args[i];
    }
    double ans = 0;
    auto p = _data->begin();
    while (p != nullptr) {
        double tans = p->data->coefficient;
        for(std::size_t i = 0; i < p->data->degrees.size(); ++i) {
            if (p->data->degrees[i] == 0) continue;
            tans *= _pow(args_data[i], p->data->degrees[i]);
        }
        ans += tans;
        p = p->next;
    }
    return ans;
}
bool Polynomial::is_multiple() {
    return _args.size() > 1;
}
bool Polynomial::is_zero(bool arg) {
    if (_data->size() == 0) return true;
    if (_data->size() != 1) return false;
    for(std::size_t i = 0; i < _data->begin()->data->degrees.size(); ++i) {
        if (_data->begin()->data->degrees[i] != 0) return false;
    }
    return !arg || _data->begin()->data->coefficient == 0;
}
Polynomial Polynomial::derivative(int arg) {
    if (is_zero()) {
        DoubleList<Element>* new_data = new DoubleList<Element>();
        new_data->push_back(new Element(0, std::vector<int>(26, 0)));
        std::vector<int> new_args;
        return Polynomial(new_data, new_args);
    }
    DoubleList<Element>* new_data = new DoubleList<Element>();
    std::vector<int> new_args;
    auto p = _data->begin();
    while (p != nullptr) {
        if (p->data->degrees[arg] != 0) {
            Element elem = *p->data;
            elem.coefficient *= elem.degrees[arg];
            elem.degrees[arg]--;
            new_data->push_back(new Element(elem));
        }
        p = p->next;
    }
    if (new_data->size() == 0) {
        new_data->push_back(new Element(0, std::vector<int>(26, 0)));
    }
    new_args = _get_args(new_data);
    return Polynomial(new_data, new_args);
}
std::pair<Polynomial, Polynomial> Polynomial::div(Polynomial sec) {
    if (is_multiple() || sec.is_multiple()) throw Exception("Функция не применима для многочленов от нескольких перемененых.");
    if (_args.size() > 0 && sec._args.size() > 0 && _args[0] != sec._args[0]) throw Exception("Аргументы выбранных функций не совпадают.");
    if (sec.is_zero(true)) throw Exception("Попытка деления на 0.");
    Polynomial next(*this);
    Polynomial a(0), b(0);
    while (true) {
        if (next.is_zero(true)) break;
        if (*next._data->begin()->data < *sec._data->begin()->data) {
            b = next;
            break;
        }
        if (next._args.size() == 0) {
            auto tmp = Polynomial(next._data->begin()->data->coefficient / sec._data->begin()->data->coefficient);
            a = a + tmp;
            break;
        }
        DoubleList<Element>* tmp_list = new DoubleList<Element>();
        std::vector<int> tmp_vec(26, 0);
        tmp_vec[next._args[0]] = next._data->begin()->data->degrees[next._args[0]] - sec._data->begin()->data->degrees[next._args[0]];
        tmp_list->push_back(new Element(next._data->begin()->data->coefficient / sec._data->begin()->data->coefficient, tmp_vec));
        Polynomial tmp(tmp_list, _args);
        a = a + tmp;
        tmp = sec * tmp;
        next = next - tmp;
    }

    return {a, b};
}
std::vector<double> Polynomial::get_roots() {
    if (is_multiple()) throw Exception("Функция не применима для многочленов от нескольких перемененых.");
    std::vector<double> ans;
    if (_args.size() == 0) return ans;
    for(int i = 0; i * i <= std::fabs(_data->back()->coefficient); ++i) {
        if (_check_root(i)) ans.push_back(i);
        if (_check_root(-i)) ans.push_back(-i);
        if (i != 0 && _check_root(_data->back()->coefficient / i)) ans.push_back(_data->back()->coefficient / i);
        if (i != 0 && _check_root(-_data->back()->coefficient / i)) ans.push_back(-_data->back()->coefficient / i);
    }
    std::sort(ans.begin(), ans.end());
    ans.erase(std::unique(ans.begin(), ans.end()), ans.end());
    return ans;
}
Polynomial Polynomial::operator+(Polynomial& sec) {
    DoubleList<Element>* new_data = new DoubleList<Element>();
    auto p = _data->begin(), q = sec._data->begin();
    while (p != nullptr || q != nullptr) {
        if (p == nullptr) {
            if (q->data->coefficient != 0)
                new_data->push_back(new Element(*q->data));
            q = q->next;
            continue;
        }
        if (q == nullptr) {
            if (p->data->coefficient != 0)
                new_data->push_back(new Element(*p->data));
            p = p->next;
            continue;
        }
        if (p->data->equals(*q->data)) {
            if (p->data->coefficient + q->data->coefficient != 0)
                new_data->push_back(new Element(p->data->coefficient + q->data->coefficient, p->data->degrees));
            p = p->next;
            q = q->next;
            continue;
        }
        if (*p->data > *q->data) {
            if (p->data->coefficient != 0)
                new_data->push_back(new Element(*p->data));
            p = p->next;
        } else {
            if (q->data->coefficient != 0)
                new_data->push_back(new Element(*q->data));
            q = q->next;
        }
    }
    if (new_data->size() == 0) new_data->push_back(new Element(0, std::vector<int>(26, 0)));
    std::vector<int> args = _get_args(new_data);
    return Polynomial(new_data, args);
}
Polynomial Polynomial::operator-(Polynomial& sec) {
    DoubleList<Element>* new_data = new DoubleList<Element>();
    auto p = _data->begin(), q = sec._data->begin();
    while (p != nullptr || q != nullptr) {
        if (p == nullptr) {
            auto el = new Element(*q->data);
            el->coefficient = -el->coefficient;
            new_data->push_back(el);
            q = q->next;
            continue;
        }
        if (q == nullptr) {
            auto el = new Element(*p->data);
            new_data->push_back(el);
            p = p->next;
            continue;
        }
        if (p->data->equals(*q->data)) {
            if (p->data->coefficient - q->data->coefficient != 0)
                new_data->push_back(new Element(p->data->coefficient - q->data->coefficient, p->data->degrees));
            p = p->next;
            q = q->next;
            continue;
        }
        if (*p->data > *q->data) {
            new_data->push_back(new Element(*p->data));
            p = p->next;
        } else {
            auto el = new Element(*q->data);
            el->coefficient = -el->coefficient;
            new_data->push_back(el);
            q = q->next;
        }
    }
    std::vector<int> args = _get_args(new_data);
    return Polynomial(new_data, args);
}
Polynomial Polynomial::operator*(Polynomial& sec) {
    DoubleList<Element>* new_data = new DoubleList<Element>();

    auto p = _data->begin();
    while (p != nullptr) {
        auto q = sec._data->begin();
        while (q != nullptr) {
            if (p->data->coefficient * q->data->coefficient != 0)
                new_data->push_back(new Element(*p->data * *q->data));
            q = q->next;
        }
        p = p->next;
    }
    new_data->sort();
    std::vector<int> args = _get_args(new_data);
    return Polynomial(new_data, args);
}
bool Polynomial::operator==(Polynomial& sec) {
    if (_data->size() != sec._data->size()) return false;
    auto p = _data->begin();
    auto q = sec._data->begin();
    while (p != nullptr && q != nullptr) {
        if (*p->data != *q->data) return false;
        p = p->next, q = q->next;
    }
    return true;
}
bool Polynomial::operator!=(Polynomial& sec) {
    return !(*this == sec);
}
