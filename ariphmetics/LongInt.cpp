#include "LongInt.h"

using namespace std;

void LongInt::removeZeroes() {
	size_t index = 0;

	while (index < digits.size() - 1 && digits[index] == 0)
		index++;

	vector<int> newDigits;

	while (index < digits.size()) {
		newDigits.push_back(digits[index]);
		index++;
	}

	digits = newDigits;

	if (digits.size() == 1 && digits[0] == 0)
		sign = 1;
}

LongInt LongInt::mult2() const {
	LongInt res;

	res.sign = sign;
	res.digits = vector<int>(digits.size() + 1, 0);

	for (size_t i = 0; i < digits.size(); i++)
		res.digits[i + 1] = digits[i];

	int carry = 0;

	for (int i = res.digits.size() - 1; i >= 0; i--) {
		int curr = carry + res.digits[i] * 2;
		res.digits[i] = curr % 10;
		carry = curr / 10;
	}

	res.removeZeroes();

	return res;
}

LongInt LongInt::div2() const {
	LongInt res;

	res.sign = sign;
	res.digits = vector<int>(digits.size(), 0);

	int carry = 0;

	for (size_t i = 0; i < res.digits.size(); i++) {
		int curr = digits[i] + carry * 10;
		res.digits[i] = curr / 2;
		carry = curr % 2;
	}

	res.removeZeroes();

	return res;
}

// конструктор по умолчанию (создаёт нулевое число)
LongInt::LongInt() {
	sign = 1;
	digits = vector<int>(1, 0);
}

LongInt::LongInt(long long v) {
	sign = v < 0 ? -1 : 1;
	v *= sign;

	long long tmp = v;
	size_t len = 0;

	while (tmp) {
		len++;
		tmp /= 10;
	}

	if (len) {
		digits = vector<int>(len, 0);

		size_t index = len - 1;

		while (v) {
			digits[index--] = v % 10;
			v /= 10;
		}
	}
	else
		digits = vector<int>(1, 0);
}

LongInt::LongInt(const std::string& s) {
	size_t index = 0;

	if (s[0] == '-') {
		sign = -1;
		index = 1;
	}
	else
		sign = 1;

	digits = vector<int>();
	
	while (index < s.length() && s[index] >= '0' && s[index] <= '9')
		digits.push_back(s[index++] - '0');
	
	if (index < s.length() || s.length() == 0 || s == "-")
		throw string("LongInt(const string& s) - s is not valid integer number");
}

LongInt::LongInt(const LongInt& n) {
	sign = n.sign;
	digits = vector<int>(n.digits.size());

	for (size_t i = 0; i < n.digits.size(); i++)
		digits[i] = n.digits[i];
}


bool LongInt::operator>(const LongInt& n) const {
	if (sign != n.sign)
		return sign > n.sign;

	if (digits.size() != n.digits.size())
		return (digits.size() > n.digits.size()) ^ (sign == -1);

	for (size_t i = 0; i < digits.size(); i++)
		if (digits[i] != n.digits[i])
			return (digits[i] > n.digits[i]) ^ (sign == -1);
	
	return false;
}

bool LongInt::operator<(const LongInt& n) const {
	if (sign != n.sign)
		return sign < n.sign;

	if (digits.size() != n.digits.size())
		return (digits.size() < n.digits.size()) ^ (sign == -1);

	for (size_t i = 0; i < digits.size(); i++)
		if (digits[i] != n.digits[i])
			return (digits[i] < n.digits[i]) ^ (sign == -1);
	
	return false;
}

bool LongInt::operator==(const LongInt& n) const {
	if (sign != n.sign)
		return false;

	if (digits.size() != n.digits.size())
		return false;

	for (size_t i = 0; i < digits.size(); i++)
		if (digits[i] != n.digits[i])
			return false;
	
	return true;
}

bool LongInt::operator!=(const LongInt& n) const {
	return !(*this == n);
}

bool LongInt::operator>=(const LongInt& n) const {
	return (*this > n) || (*this == n);
}

bool LongInt::operator<=(const LongInt& n) const {
	return (*this < n) || (*this == n);
}


LongInt& LongInt::operator=(const LongInt& n) {
	if (this == &n)
		return *this;

	sign = n.sign;
	digits = vector<int>(n.digits.size());

	for (size_t i = 0; i < n.digits.size(); i++)
		digits[i] = n.digits[i];

	return *this;
}

LongInt LongInt::operator-() const {
	LongInt tmp(*this);
	tmp.sign = -sign;

	return tmp;
} 


LongInt LongInt::operator+(const LongInt& n) const {
	if (sign == n.sign) { // если знаки одинаковые, то выполняем сложение
		size_t len = 1 + max(digits.size(), n.digits.size());

		LongInt res;

		res.sign = sign;
		res.digits = vector<int>(len);

		for (int i = digits.size() - 1; i >= 0; i--)
			res.digits[len - digits.size() + i] = digits[i];

		for (int i = n.digits.size() - 1; i >= 0; i--)
			res.digits[len - n.digits.size() + i] += n.digits[i];

		for (size_t i = len - 1; i > 0; i--) {
			res.digits[i - 1] += res.digits[i] / 10;
			res.digits[i] %= 10;
		}

		res.removeZeroes();

		return res;
	}

	if (sign == -1)
		return n - (-(*this));
	
	return *this - (-n);
}

LongInt LongInt::operator-(const LongInt& n) const {
	if (sign == 1 && n.sign == 1) { // если оба числа положительны
		size_t len = 1 + max(digits.size(), n.digits.size());
		bool cmp = *this > n;

		LongInt a(cmp ? *this : n);
		LongInt b(cmp ? n : *this);
		LongInt res;

		res.sign = cmp ? 1 : -1;
		res.digits = vector<int>(len);

		for (int i = a.digits.size() - 1; i >= 0; i--)
			res.digits[len - a.digits.size() + i] = a.digits[i];

		for (int i = b.digits.size() - 1; i >= 0; i--)
			res.digits[len - b.digits.size() + i] -= b.digits[i];

		for (size_t i = len - 1; i > 0; i--) {
			if (res.digits[i] < 0) {
				res.digits[i] += 10;
				res.digits[i - 1]--;
			}
		}

		res.removeZeroes();

		return res;
	}

	if (sign == -1 && n.sign == -1)
		return (-n) - (-(*this));
	
	return *this + (-n);
}

LongInt LongInt::operator*(const LongInt& n) const {
	size_t len = 1 + digits.size() + n.digits.size();
	LongInt res;

	res.sign = sign * n.sign;
	res.digits = vector<int>(len, 0);

	for (size_t i = 0; i < digits.size(); i++)
		for (size_t j = 0; j < n.digits.size(); j++)
			res.digits[len - 1 - i - j] += digits[digits.size() - 1 - i] * n.digits[n.digits.size() - 1 - j];
	
	for (size_t i = len - 1; i > 0; i--) {
		res.digits[i - 1] += res.digits[i] / 10;
		res.digits[i] %= 10;
	}

	res.removeZeroes();

	return res;
}

LongInt LongInt::operator/(const LongInt& n) const {
	LongInt z = 1;

	LongInt mod(*this);
	LongInt b(n);

	mod.sign = 1;
	b.sign = 1;

	long long k = 0;

	while (mod >= b) {
		b = b.mult2();
		z = z.mult2();
		k++;
	}

	LongInt div;

	while (k > 0) {
		b = b.div2();
		z = z.div2();
		k--;

		if (mod >= b) {
			mod = mod - b;
			div = div + z;
		}
	}

	div.sign = sign * n.sign;
	div.removeZeroes();

	return div;
}

LongInt LongInt::operator%(const LongInt& n) const {
	LongInt mod(*this);
	LongInt b(n);

	mod.sign = 1;
	b.sign = 1;

	long long k = 0;

	while (mod >= b) {
		b = b.mult2();
		k++;
	}

	while (k > 0) {
		b = b.div2();

		k--;

		if (mod >= b)
			mod = mod - b;
	}

	mod.sign = sign;
	mod.removeZeroes();

	return mod;
}


LongInt& LongInt::operator+=(const LongInt& n) {
	return (*this = *this + n);
}

LongInt& LongInt::operator-=(const LongInt& n) {
	return (*this = *this - n);
}

LongInt& LongInt::operator*=(const LongInt& n) {
	return (*this = *this * n);
}

LongInt& LongInt::operator/=(const LongInt& n) {
	return (*this = *this / n);
}

LongInt& LongInt::operator%=(const LongInt& n) {
	return (*this = *this % n);
}


LongInt LongInt::operator++(int) {
	LongInt tmp(*this);
	*this += 1;

	return tmp;
}

LongInt LongInt::operator--(int) {
	LongInt tmp(*this);
	*this -= 1;

	return tmp;
}

LongInt& LongInt::operator++() {
	return (*this += 1);
}

LongInt& LongInt::operator--() {
	return (*this -= 1);
}

bool LongInt::isEven() const {
	return digits[digits.size() - 1] % 2 == 0;
}

bool LongInt::isOdd() const {
	return digits[digits.size() - 1] % 2 == 1;
}

LongInt LongInt::factorial() const {
	if (sign == -1)
		throw string("LongInt::factorial() - number must be positive");

	LongInt fact = 1;

	for (LongInt i = 2; i <= *this; i++)
		fact *= i;

	return fact;
}

string LongInt::toString() const {
	string s;

	if (sign == -1)
		s += '-';

	for (size_t i = 0; i < digits.size(); i++)
		s += to_string(digits[i]);

	return s;
}

// ввод из потока
istream& operator>>(istream &fs, LongInt &n) {
	char c;
	n.digits = vector<int>(0);

	fs.get(c);

	while (c == ' ' || c == '\n' || c == '\t')
		fs.get(c);

	if (c == '-') {
		n.sign = -1;
		fs.get(c);
	}
	else
		n.sign = 1;

	while (fs && c >= '0' && c <= '9') {
		n.digits.push_back(c - '0');
		fs.get(c);
	}

	return fs;
}

// вывод в поток
ostream& operator<<(ostream &fs, const LongInt &n) {
	if (n.sign == -1)
		fs << '-';

	for (size_t i = 0; i < n.digits.size(); i++)
		fs << n.digits[i];
	
	return fs;
}