#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

using namespace std;

class BigInt {
private:
    string value;
    bool isPositive;

public:
    BigInt(const string& val = "0") : value(val), isPositive(true) {
        if (value[0] == '-') {
            isPositive = false;
            value = value.substr(1);
        }
        removeLeadingZeros();
    }

    BigInt(int val) {
        isPositive = val >= 0;
        value = to_string(abs(val));
    }

    void removeLeadingZeros() {
        while (value.size() > 1 && value[0] == '0') {
            value.erase(0, 1);
        }
        if (value == "0") {
            isPositive = true;
        }
    }

    bool operator>=(const BigInt& other) const {
        if (isPositive != other.isPositive) return isPositive;
        if (value.size() != other.value.size()) return value.size() > other.value.size();
        return value >= other.value;
    }

    BigInt ADD(const BigInt& other) const {
        string result = addStrings(value, other.value);
        return BigInt(result);
    }

    BigInt SUBTRACT(const BigInt& other) const {
        string result = subtractStrings(value, other.value);
        return BigInt(result);
    }

    BigInt MULTIPLY(const BigInt& other) const {
        if (value == "0" || other.value == "0") return BigInt("0");
        string result = multiplyStrings(value, other.value);
        bool resultSign = isPositive == other.isPositive;
        return BigInt((resultSign ? "" : "-") + result);
    }

    BigInt KARATSUBA_MULTIPLY(const BigInt& other) const {
        if (value == "0" || other.value == "0") return BigInt("0");
        string result = karatsubaMultiply(value, other.value);
        bool resultSign = isPositive == other.isPositive;
        return BigInt((resultSign ? "" : "-") + result);
    }

    BigInt DIVIDE(const BigInt& other) const {
        if (other.value == "0") throw invalid_argument("Division by zero");
        bool resultSign = isPositive == other.isPositive;
        string quotient = divideStrings(value, other.value);
        return BigInt((resultSign ? "" : "-") + quotient);
    }

    BigInt POWER(int exponent) const {
        if (exponent < 0) throw invalid_argument("Exponent must be non-negative");
        if (exponent == 0) return BigInt("1");
        BigInt base = *this;
        BigInt result("1");

        while (exponent > 0) {
            if (exponent % 2 == 1) {
                result = result.MULTIPLY(base);
            }
            base = base.MULTIPLY(base);
            exponent /= 2;
        }

        return result;
    }

    BigInt FACTORIAL() const {
        if (value == "0") return BigInt("1");
        BigInt result("1");
        BigInt counter("1");
        BigInt one("1");

        while (counter.value != this->value) {
            result = result.MULTIPLY(counter);
            counter = counter.ADD(one);
        }
        result = result.MULTIPLY(counter);
        return result;
    }

    friend ostream& operator<<(ostream& os, const BigInt& bigInt) {
        os << (bigInt.isPositive ? "" : "-") << bigInt.value;
        return os;
    }

private:
    string multiplyStrings(const string& num1, const string& num2) const {
        int n1 = num1.size(), n2 = num2.size();
        vector<int> result(n1 + n2, 0);

        for (int i = n1 - 1; i >= 0; --i) {
            for (int j = n2 - 1; j >= 0; --j) {
                int mul = (num1[i] - '0') * (num2[j] - '0');
                int sum = mul + result[i + j + 1];

                result[i + j + 1] = sum % 10;
                result[i + j] += sum / 10;
            }
        }

        string resultStr;
        for (int num : result) {
            if (!(resultStr.empty() && num == 0)) {
                resultStr.push_back(num + '0');
            }
        }

        return resultStr.empty() ? "0" : resultStr;
    }

    string karatsubaMultiply(const string& x, const string& y) const {
        int n = max(x.size(), y.size());

        if (n == 1) return multiplyStrings(x, y);

        n = (n / 2) + (n % 2);
        string x1 = x.size() > n ? x.substr(0, x.size() - n) : "0";
        string x0 = x.substr(x.size() - n);
        string y1 = y.size() > n ? y.substr(0, y.size() - n) : "0";
        string y0 = y.substr(y.size() - n);

        BigInt X1(x1), X0(x0), Y1(y1), Y0(y0);
        BigInt z2 = X1.KARATSUBA_MULTIPLY(Y1);
        BigInt z0 = X0.KARATSUBA_MULTIPLY(Y0);
        BigInt z1 = (X1.ADD(X0)).KARATSUBA_MULTIPLY(Y1.ADD(Y0)).SUBTRACT(z2).SUBTRACT(z0);

        string result = z2.value + string(2 * n, '0');
        result = addStrings(result, z1.value + string(n, '0'));
        result = addStrings(result, z0.value);

        return result;
    }


    string divideStrings(const string& dividend, const string& divisor) const {
        if (divisor == "0") throw invalid_argument("Division by zero");

        string result;
        BigInt tempDividend("0");
        for (char digit : dividend) {
            tempDividend = tempDividend.ADD(BigInt(string(1, digit)));
            int count = 0;
            while (tempDividend >= BigInt(divisor)) {
                tempDividend = tempDividend.SUBTRACT(BigInt(divisor));
                ++count;
            }
            result.push_back(count + '0');
        }

        return result.empty() ? "0" : result;
    }

    string addStrings(const string& num1, const string& num2) const {
        string result;
        int carry = 0;
        int n1 = num1.size() - 1;
        int n2 = num2.size() - 1;

        while (n1 >= 0 || n2 >= 0 || carry) {
            int sum = carry;
            if (n1 >= 0) sum += num1[n1--] - '0';
            if (n2 >= 0) sum += num2[n2--] - '0';

            result.push_back(sum % 10 + '0');
            carry = sum / 10;
        }

        reverse(result.begin(), result.end());
        return result;
    }

    string subtractStrings(const string& num1, const string& num2) const {
        string result;
        int borrow = 0;
        int n1 = num1.size() - 1;
        int n2 = num2.size() - 1;

        while (n1 >= 0 || n2 >= 0 || borrow) {
            int diff = (num1[n1--] - '0') - (n2 >= 0 ? num2[n2--] - '0' : 0) - borrow;
            borrow = diff < 0;
            result.push_back((diff + (borrow ? 10 : 0)) + '0');
        }

        reverse(result.begin(), result.end());
        return result;
    }
};

int main() {
    BigInt num1("252435234534");
    BigInt num2("123456");
    int exponent = 5;

    BigInt mulResult = num1.MULTIPLY(num2);
    BigInt karatsubaResult = num1.KARATSUBA_MULTIPLY(num2);
    BigInt divResult = num1.DIVIDE(num2);
    BigInt powerResult = num1.POWER(exponent);
    BigInt factorialResult = BigInt(100).FACTORIAL();

    cout << "Multiplication (standard): " << mulResult << endl;
    cout << "Karatsuba Multiplication: " << karatsubaResult << endl;
    cout << "Division: " << divResult << endl;
    cout << "Power (num1 ^ " << exponent << "): " << powerResult << endl;
    cout << "Factorial (100!): " << factorialResult << endl;

    return 0;
}

