#ifndef BIGNUM_HPP
#define BIGNUM_HPP

#include <string>
#include <vector>

class Bignum {
public:
    Bignum();
    Bignum(const std::string& num);

    Bignum operator+(const Bignum& other) const;
    Bignum operator-(const Bignum& other) const;
    Bignum operator*(const Bignum& other) const;
    Bignum operator/(const Bignum& other) const;
    Bignum operator%(const Bignum& other) const;

    Bignum modexp(const Bignum& exponent, const Bignum& modulus) const;

    std::string to_string();
    std::string fromEncrypted();

private:
    std::vector<int> digits;
    bool isZero() const;

    void removeLeadingZeros();
    void addLeadingZeros();
    
    Bignum multiplyBySingleDigit(int digit) const;

    bool operator<(const Bignum& other) const;
    bool operator==(const Bignum& other) const;
};

#endif
