#include <iostream>
#include <vector>

class BigInt {
public:
    std::vector<int> digits;

    BigInt() { }
    
    BigInt(std::string s) {
        for (int i = s.length() - 1; i >= 0; i--) {
            if (isdigit(s[i])) {
                digits.push_back(s[i] - '0');
            }
        }
        trim();
    }

    BigInt(unsigned long long v) {
        if (v == 0) digits.push_back(0);
        while (v > 0) {
            digits.push_back(v % 10);
            v /= 10;
        }
    }

    void print() const {
        for (int i = digits.size() - 1; i >= 0; i--) {
            std::cout << digits[i];
        }
    }

    void trim() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
    }

    bool is_zero() const {
        return digits.size() == 1 && digits[0] == 0;
    }

    BigInt operator+(const BigInt& other) const {
        BigInt result;

        int carry = 0;
        int n = std::max(digits.size(), other.digits.size());

        for (int i = 0; i < n || carry; i++) {
            int sum = carry;
            if (i < digits.size()) sum += digits[i];
            if (i < other.digits.size()) sum += other.digits[i];
            result.digits.push_back(sum % 10);
            carry = sum / 10;
        }
        result.trim();
        return result;
    }

    BigInt operator-(const BigInt& other) const {
        BigInt result;

        int carry = 0;

        for (int i = 0; i < digits.size(); i++) {
            int diff = digits[i] - carry - (i < other.digits.size() ? other.digits[i] : 0);
            if (diff < 0) {
                diff += 10;
                carry = 1;
            } else {
                carry = 0;
            }
            result.digits.push_back(diff);
        }
        result.trim();
        return result;
    }

    BigInt operator*(const BigInt& other) const {
        BigInt result;
        result.digits.resize(digits.size() + other.digits.size(), 0);

        for (int i = 0; i < digits.size(); i++) {
            int carry = 0;
            for (int j = 0; j < other.digits.size() || carry; j++) {
                int cur = result.digits[i + j] + digits[i] * (j < other.digits.size() ? other.digits[j] : 0) + carry;
                result.digits[i + j] = cur % 10;
                carry = cur / 10;
            }
        }
        result.trim();
        return result;
    }

    BigInt square() const {
        return (*this) * (*this);
    }

    BigInt operator%(const BigInt& other) const {
        if (other.is_zero()) throw std::invalid_argument("Division by zero");
        
        BigInt mod = 0;
        for (int i = digits.size() - 1; i >= 0; i--) {
            mod = mod * 10 + digits[i];
            while (mod >= other) {
                mod = mod - other;
            }
        }
        return mod;
    }

private:
    bool operator>=(const BigInt& other) const {
        if (digits.size() != other.digits.size()) {
            return digits.size() > other.digits.size();
        }
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] != other.digits[i]) {
                return digits[i] > other.digits[i];
            }
        }
        return true;
    }
};

BigInt modular_pow(BigInt a, BigInt b, const BigInt& m) {
    BigInt result = 1;
    a = a % m;

    while (!b.is_zero()) {
        if (b.digits[0] % 2 == 1) {
            result = (result * a) % m;
        }
        
        a = a.square() % m;
        
        BigInt b_n = 0;
        b_n.digits.resize(b.digits.size(), 0);
        int carry = 0;

        for (int i = b.digits.size() - 1; i >= 0; i--) {
            int cur = b.digits[i] + carry * 10;
            b_n.digits[i] = cur / 2; 
            carry = cur % 2;
        }
        b_n.trim();
        b = b_n;
    }
    return result;
}

int main() {
    BigInt a("32453");
    BigInt b("2");
    BigInt m("10");

    std::cout << "Число a: "; a.print(); std::cout << std::endl;
    std::cout << "Число b: "; b.print(); std::cout << std::endl;
    std::cout << "Модуль m: "; m.print(); std::cout << std::endl << std::endl;

    BigInt res_add = a + b;
    std::cout << "1. a + b = "; res_add.print(); std::cout << std::endl;

    BigInt res_mult = a * b;
    std::cout << "2. a * b = "; res_mult.print(); std::cout << std::endl;

    BigInt res_sqr = a.square();
    std::cout << "3. a^2 = "; res_sqr.print(); std::cout << std::endl;

    BigInt res_mod = a % m;
    std::cout << "4. a mod m = "; res_mod.print(); std::cout << std::endl;

    BigInt res_mod_pow = modular_pow(a, b, m);
    std::cout << "5. a^b (mod m) = "; res_mod_pow.print(); std::cout << std::endl;

    return 0;
}
