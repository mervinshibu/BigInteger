/**
 * A cleaned‑up version of the old BigInt class.
 * ‑ Uses the Rule of Five to avoid memory leaks.
 * ‑ Keeps the public interface as close as possible to the original
 *   so that existing code keeps compiling.
 * ‑ Internal logic is still the same (little‑endian digit storage),
 *   but arrays are always zero‑initialised, and every owning pointer
 *   is freed exactly once.
 */

#include <iostream>
#include <string>
#include <algorithm>  // std::copy, std::fill_n, std::max

namespace detail {
    // helper that behaves like mathematical mod but works for negatives
    inline long mod(long a, long b) noexcept { return a < 0 ? -(-a % b) : a % b; }
}

class BigInt {
    long *val_ {nullptr};          // little‑endian digits (least‑significant first)
    std::size_t len_ {0};          // number of used digits

    // utility: create zero‑initialised buffer and assign to *this (caller deleted current)
    void reallocate(std::size_t new_len) {
        delete[] val_;
        val_ = new long[new_len]();   // value‑initialised → zeros
        len_ = new_len;
    }

public:
    /*–––––––– Constructors ––––––––*/
    BigInt() = default;                       // 0 by default – represented by len_ == 0
    
    BigInt(long n)               { *this = n; }
    explicit BigInt(const std::string &s) { *this = s; }

    // Copy constructor (deep copy)
    BigInt(const BigInt &other) : val_(nullptr), len_(other.len_) {
        if (len_) {
            val_ = new long[len_];
            std::copy(other.val_, other.val_ + len_, val_);
        }
    }

    // Move constructor
    BigInt(BigInt &&other) noexcept : val_(other.val_), len_(other.len_) {
        other.val_ = nullptr;
        other.len_ = 0;
    }

    // Destructor
    ~BigInt() { delete[] val_; }

    /*–––––––– Assignment operators ––––––––*/
    BigInt &operator=(const BigInt &other) {
        if (this != &other) {
            reallocate(other.len_);
            std::copy(other.val_, other.val_ + len_, val_);
        }
        return *this;
    }

    BigInt &operator=(BigInt &&other) noexcept {
        if (this != &other) {
            delete[] val_;
            val_      = other.val_;
            len_      = other.len_;
            other.val_ = nullptr;
            other.len_ = 0;
        }
        return *this;
    }

    BigInt &operator=(long n) {
        int sign = (n < 0 ? -1 : 1);
        unsigned long un = (n < 0 ? -static_cast<unsigned long>(n) : static_cast<unsigned long>(n));
        // count digits
        std::size_t digits = 1;
        for (unsigned long t = 10; t <= un; t *= 10) ++digits;
        reallocate(digits);
        for (std::size_t i = 0; i < digits; ++i) {
            val_[i] = (un % 10) * sign;
            un /= 10;
        }
        // normalise zero
        if (digits == 1 && val_[0] == 0) len_ = 0;
        return *this;
    }

    BigInt &operator=(const std::string &s) {
        int sign = 1;
        std::size_t start = 0;
        if (!s.empty() && s[0] == '-') { sign = -1; start = 1; }
        std::size_t digits = s.size() - start;
        reallocate(digits);
        for (std::size_t i = 0; i < digits; ++i) {
            val_[i] = (s[s.size() - 1 - i] - '0') * sign;
        }
        // trim leading zeros
        while (len_ && val_[len_ - 1] == 0) --len_;
        return *this;
    }

    /*–––––––– Display helpers ––––––––*/
    void print(std::ostream &os = std::cout) const {
        if (len_ == 0) { os << 0; return; }
        short sign = val_[len_ - 1] < 0 ? -1 : 1;
        os << val_[len_ - 1];
        for (std::size_t i = len_ - 1; i-- > 0;) os << val_[i] * sign;
    }

    friend std::ostream &operator<<(std::ostream &os, const BigInt &b) {
        b.print(os); return os;
    }

    friend std::istream &operator>>(std::istream &is, BigInt &b) {
        std::string s; is >> s; b = s; return is;
    }

    /*–––––––– Addition ––––––––*/
    friend BigInt operator+(const BigInt &a, const BigInt &b) {
        BigInt sum;
        std::size_t max_len = std::max(a.len_, b.len_);
        sum.reallocate(max_len + 1);
        long carry = 0;
        for (std::size_t i = 0; i < sum.len_; ++i) {
            long o1 = (i < a.len_) ? a.val_[i] : 0;
            long o2 = (i < b.len_) ? b.val_[i] : 0;
            sum.val_[i] = o1 + o2 + carry;
            carry       = sum.val_[i] / 10;
            sum.val_[i] = detail::mod(sum.val_[i], 10);
        }
        while (sum.len_ && sum.val_[sum.len_ - 1] == 0) --sum.len_;
        return sum;
    }

    /*–––––––– Multiplication ––––––––*/
    friend BigInt operator*(const BigInt &a, const BigInt &b) {
        if (a.len_ == 0 || b.len_ == 0) return BigInt();
        BigInt prod;
        prod.reallocate(a.len_ + b.len_);
        for (std::size_t i = 0; i < b.len_; ++i) {
            long carry = 0;
            for (std::size_t j = 0; j < a.len_; ++j) {
                std::size_t idx = i + j;
                long tmp   = prod.val_[idx] + b.val_[i] * a.val_[j] + carry;
                carry      = tmp / 10;
                prod.val_[idx] = detail::mod(tmp, 10);
            }
            prod.val_[i + a.len_] += carry;
        }
        while (prod.len_ && prod.val_[prod.len_ - 1] == 0) --prod.len_;
        return prod;
    }

    /*–––––––– helpers for mixing with built‑ins/strings ––––––––*/
    friend BigInt operator+(const BigInt &a, long b)        { return a + BigInt(b); }
    friend BigInt operator+(const BigInt &a, const char *s) { return a + BigInt(std::string(s)); }
    friend BigInt operator*(const BigInt &a, long b)        { return a * BigInt(b); }
    friend BigInt operator*(const BigInt &a, const char *s) { return a * BigInt(std::string(s)); }
};

int main() {
    BigInt a = 1;
    for (int i = 1; i <= 1000; ++i) a = a * i;   // 1000!
    std::cout << a << '\n';
}
