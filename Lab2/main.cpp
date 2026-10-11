#include <iostream>
#include <vector>
#include <random>

std::mt19937_64 rng{std::random_device{}()};

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

    bool operator==(const BigInt& other) const { 
        return digits == other.digits; 
    }

    bool operator!=(const BigInt& other) const { 
        return digits != other.digits; 
    }

    bool operator<(const BigInt& o) const { 
        return !(*this >= o); 
    }

    BigInt div_2() const {
        BigInt r;
        r.digits.assign(digits.size(), 0);
        int carry = 0;
        for (int i = (int)digits.size() - 1; i >= 0; i--) {
            int cur = digits[i] + carry * 10;
            r.digits[i] = cur / 2;
            carry = cur % 2;
        }
        r.trim();
        return r;
    }

    bool is_odd() const { 
        return digits[0] % 2 == 1; 
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

bool gcd(BigInt a, BigInt b) {
    while (b != 0) {
        BigInt t = b;
        b = a % b;
        a = t;
    }
    
    if (a == 1) {
        return true;
    }
    return false;
}

BigInt random_between(BigInt min, BigInt max) {
    BigInt range = max - min + 1;
    BigInt s;
    int len = range.digits.size() + 10;
    for (int i = 0; i < len; i++) { 
        s = s + char('0' + rng() % 10);
    }
    return min + (s % range);
}

bool is_prime_fermat(const BigInt& n) {
    BigInt a;
    do {
        a = random_between(1, n);
    } while (a % n == 0);

    BigInt x = modular_pow(a, n - 1, n);

    if (x == 1) {
        return true;
    }
    return false;
}

bool is_prime_miller_rabin(const BigInt& n) {
    int rounds = 10;
 
    BigInt d = (n - 1).div_2();
    int s = 1;
    while (!d.is_odd()) {
        d = d.div_2();
        s++;
    }

    for (int i = 0; i < rounds; i++) {
        BigInt a = random_between(2, n - 2);
        BigInt x = modular_pow(a, d, n);

        BigInt y;
        for (int j = 0; j < s; j++) {
            y = x.square() % n;

            if (y == 1 && x != 1 && x != n - 1) {
                return false;
            }
            x = y;
        }

        if (y != 1) {
            return false;
        }
    }
    return true;
}

BigInt random_num(int digits) {
    std::string s;

    s += char('1' + rng() % 9);
    for (int i = 1; i < digits - 1; i++){
        s += char('0' + rng() % 10);
    }
    s += char('1' + 2 * (rng() % 5));
    return BigInt(s);
}

BigInt generate_prime(int digits) {
    while (true) {
        BigInt n = random_num(digits);
        if (is_prime_fermat(n)) {
            if (is_prime_miller_rabin(n)) {
                return n;
            }
        }
    }
}

int main() {
    int digits = 3;
    BigInt p = generate_prime(digits);
    BigInt q;
    do {
        q = generate_prime(digits);
    }while (q == p);

    std::cout << "p = "; p.print(); std::cout << std::endl;
    std::cout << "q = "; q.print(); std::cout << std::endl;
}