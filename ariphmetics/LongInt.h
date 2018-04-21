#include <iostream>
#include <vector>
#include <string>

class LongInt {
	int sign; // знак числа: 1 - положительное, -1 - отрицательное
	std::vector<int> digits; // вектор цифр

	void removeZeroes(); // удаление незначащих нулей
	LongInt mult2() const; // умножение на 2
	LongInt div2() const; // деление на 2

public:
	LongInt();
	LongInt(long long v);
	LongInt(const std::string& s);
	LongInt(const LongInt& n);

	LongInt& operator=(const LongInt& n); // оператор присваивания

	bool operator>(const LongInt& n) const; // проверка на "больше"
	bool operator<(const LongInt& n) const; // проверка на "меньше"
	bool operator==(const LongInt& n) const; // проверка на равенство
	bool operator!=(const LongInt& n) const; // проверка на неравенство
	bool operator>=(const LongInt& n) const; // проверка на "больше или равно"
	bool operator<=(const LongInt& n) const; // проверка на "меньше или равно"

	LongInt operator-() const; // унарный минус - смена знака

	LongInt operator+(const LongInt& n) const; // сложение двух чисел
	LongInt operator-(const LongInt& n) const; // вычитание двух чисел
	LongInt operator*(const LongInt& n) const; // умножение двух чисел
	LongInt operator/(const LongInt& n) const; // деление двух чисел
	LongInt operator%(const LongInt& n) const; // остаток от деления двух чисел

	LongInt& operator+=(const LongInt& n); // сложение двух чисел (краткая форма)
	LongInt& operator-=(const LongInt& n); // вычитание двух чисел (краткая форма)
	LongInt& operator*=(const LongInt& n); // умножение двух чисел (краткая форма)
	LongInt& operator/=(const LongInt& n); // деление двух чисел (краткая форма)
	LongInt& operator%=(const LongInt& n); // остаток от деления двух чисел (краткая форма)

	LongInt operator++(int); // постфиксная форма инкремента
	LongInt operator--(int); // постфиксная форма декремента

	LongInt& operator++(); // префиксная форма инкремента
	LongInt& operator--(); // префиксная форма декремента

	bool isEven() const; // проверка на чётность
	bool isOdd() const; // проверка на нечётность

	LongInt factorial() const; // факториал числа

	std::string toString() const;

	friend std::istream& operator>>(std::istream &fs, LongInt &n);
	friend std::ostream& operator<<(std::ostream &fs, const LongInt &n);
};