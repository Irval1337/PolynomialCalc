## PolynomialCalc
Многофункциональный калькулятор многочленов, написанный с использованием фреймворка QT на языке C++.

Основной функционал:
* Хранение всех многочленов в виде базы данных, представленной в виде самописного двусвязного списка. Каждый многочлен также представляет из себя двусвязный список из переменных и их степеней.
* Поддержка работы с многочленами от нескольких переменных.
* Проверка введенных многочленов с помощью детерминированного конечного автомата.
* Сохранение и загрузка базы данных в/из файл(а).

### Операции
1. Вычисление значение в точке (для многочленов многих переменных запрашиваются значения всех переменных).
2. Сравнение двух многочленов на равенство.
3. Сложение двух многочленов. O(n)
4. Умножение двух многочленов. O(n^2*logn)
5. Вычисление n-ой производной (n запрашивается). Для многочлена многих переменных вычисляются частные производные (переменная, по которой дифференцировать, также запрашивается).
6. Деление многочленов. Результатом этой операции являются два многочлена: частное и остаток. Данная операция не применима к многочленам многих переменных.
7. Нахождение целых корней многочлена. Используется теорема о свободном члене. Данная операция не применима к многочленам многих переменных.

Скриншоты работы:

![example_1](https://github.com/Irval1337/PolynomialCalc/blob/main/Screenshot_10.png)
