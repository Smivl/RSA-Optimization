#include "bignum.hpp"

#include <stdexcept>
#include <iostream>

Bignum::Bignum() : digits(1, 0) {}

Bignum::Bignum(const std::string& num) {
    digits.reserve(154);

    for (char c : num) {
        if (c < '0' || c > '9') throw std::invalid_argument("Error: \"" + num + "\" is not an unsigned integer");
        digits.push_back(c - '0');
    }
    removeLeadingZeros();
}

Bignum Bignum::operator+(const Bignum& other) const {
    Bignum result;
    result.digits.clear();
    

    int carry = 0, i = digits.size() - 1, j = other.digits.size() - 1;

    while (i >= 0 || j >= 0 || carry) {
        int sum = carry + (i >= 0 ? digits[i--] : 0) + (j >= 0 ? other.digits[j--] : 0);
        result.digits.insert(result.digits.begin(), sum % 10);
        carry = sum / 10;
    }

    return result;
}

Bignum Bignum::operator-(const Bignum& other) const {
    if (*this < other) throw std::runtime_error("Unsupported: Negative number");

    Bignum result;

    result.digits.clear();
    result.digits.resize(digits.size(), 0);

    int borrow = 0;
    int i = digits.size() - 1;
    int j = other.digits.size() - 1;

    for (int k = result.digits.size()-1; k >= 0; --k) {
        int diff = digits[i--] - (j >= 0 ? other.digits[j--] : 0) - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.digits[k] = diff;
    }

    result.removeLeadingZeros();
    return result;
}



Bignum Bignum::operator*(const Bignum& other) const {
    Bignum result;

    result.digits.resize(digits.size() + other.digits.size(), 0); 

    for (int i = digits.size() - 1; i >= 0; --i) {
        int carry = 0;

        for (int j = other.digits.size() - 1; j >= 0; --j) {

            int product = digits[i] * other.digits[j] + result.digits[i + j + 1] + carry;
            result.digits[i + j + 1] = product % 10;

            carry = product / 10;                   
        }

        result.digits[i] += carry;
    }

    result.removeLeadingZeros();

    return result;
}

Bignum Bignum::operator/(const Bignum& other) const {

    if (other.isZero()) throw std::runtime_error("Error: Divide by zero");

    Bignum result, remainder;

    for (int digit : digits) {

        remainder.digits.push_back(digit);
        remainder.removeLeadingZeros();

        int quotient = 0;
        while (!(remainder < other)) {
            remainder = remainder - other;
            ++quotient;
        }
        result.digits.push_back(quotient);
    }

    result.removeLeadingZeros();
    return result;
}


Bignum Bignum::operator%(const Bignum& other) const {
    if (other.isZero()) throw std::runtime_error("Error: Divide by zero");

    Bignum remainder;

    for (int digit : digits) {
        remainder.digits.push_back(digit);
        remainder.removeLeadingZeros();
        while (!(remainder < other)) {
            remainder = remainder - other;
        }
    }

    return remainder;
}


Bignum Bignum::modexp(const Bignum& exponent, const Bignum& modulus) const {

    Bignum base = *this % modulus;
    Bignum exp = exponent, result("1");

    while (!exp.isZero()) {
        if (exp.digits.back() % 2 == 1) result = (result * base) % modulus;
        exp = exp / Bignum("2");
        base = (base * base) % modulus;
    }


    return result;
}


Bignum Bignum::multiplyBySingleDigit(int digit) const {
    
    Bignum result;
    int carry = 0;

    for (int i = digits.size() - 1; i >= 0; --i) {

        int product = digits[i] * digit + carry;
        result.digits.insert(result.digits.begin(), product % 10);
        carry = product / 10;

    }
    if (carry) result.digits.insert(result.digits.begin(), carry);
    return result;
}

std::string Bignum::fromEncrypted(){
    std::string result;
    
    addLeadingZeros();
    
    for(int i = 0; i < digits.size()-1; i += 3){
        std::string character;
        bool addedC = false;
        for(int j = 0; j < 3; j++){
            if (addedC) character += std::to_string(digits[i+j]);
            else if (digits[i+j] != 0){
                character += std::to_string(digits[i+j]);
                addedC = true;
            }
        }
        result += char(std::stoi(character));
    }
    
    return result;
    
}

bool Bignum::isZero() const {
    return digits.size() == 1 && digits[0] == 0;
}

void Bignum::removeLeadingZeros() {
    while (digits.size() > 1 && digits.front() == 0) digits.erase(digits.begin());
}

void Bignum::addLeadingZeros() {
    int zeroesToAdd = 153 - digits.size();
    for(int i = 0; i < zeroesToAdd; i++) digits.insert(digits.begin(), 0);
}

bool Bignum::operator<(const Bignum& other) const {
    if (digits.size() != other.digits.size()) return digits.size() < other.digits.size();

    for (size_t i = 0; i < digits.size(); ++i)
        if (digits[i] != other.digits[i]) return digits[i] < other.digits[i];
    return false;
}

bool Bignum::operator==(const Bignum& other) const {
    return digits == other.digits;
}

std::string Bignum::to_string() {
    
    removeLeadingZeros();
    std::string result = "";
    for (int digit : digits) result += std::to_string(digit);
    return std::move(result);
}
