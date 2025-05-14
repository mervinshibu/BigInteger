 /**
 * BigInt – cleaned‑up C++ arbitrary‑precision integer
 * ----------------------------------------------------
 *  • Uses the Rule of Five – no leaks.
 *  • Little‑endian base‑10 digit storage kept from the original.
 *  • Supports +, *, **new** / and % (integer division & modulo).
 *
 *  Notes on division implementation
 *  --------------------------------
 *  The core algorithm is classic long‑division in base‑10:
 *    – We scan the dividend from most‑significant digit downwards.
 *    – At each step we multiply the running remainder by 10, push the
 *      next digit, and find the largest digit 0‑9 such that
 *      divisor * digit ≤ remainder.
 *    – Because our base is 10, that inner search is at most 10 loops.
 *  This is not the fastest possible algorithm, but it is short, clear
 *  and entirely self‑contained (no std::vector, no GMP, etc.).
 */

#include <iostream>
#include <string>
#include <algorithm>  // std::copy, std::fill_n, std::max
#include <cstdlib>     // std::abs
#include <stdexcept>   // std::runtime_error


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

void trim() { while (len_ && val_[len_ - 1] == 0) --len_; }

    static int compareAbs(const BigInt &a, const BigInt &b) {
        if (a.len_ != b.len_) return (a.len_ < b.len_) ? -1 : 1;
        for (std::size_t i = a.len_; i-- > 0;) {
            long da = std::abs(a.val_[i]);
            long db = std::abs(b.val_[i]);
            if (da != db) return (da < db) ? -1 : 1;
        }
        return 0;
    }

    static BigInt absSub(const BigInt &a, const BigInt &b) { // |a| ≥ |b|
        BigInt diff;
        diff.reallocate(a.len_);
        long borrow = 0;
        for (std::size_t i = 0; i < a.len_; ++i) {
            long av = std::abs(a.val_[i]) - borrow;
            long bv = (i < b.len_) ? std::abs(b.val_[i]) : 0;
            long d  = av - bv;
            if (d < 0) { d += 10; borrow = 1; } else borrow = 0;
            diff.val_[i] = d;
        }
        diff.trim();
        return diff;
    }

    // helper: multiply positive BigInt by one decimal digit (0‑9)
    static BigInt mulDigit(const BigInt &a, int d) {
        if (d == 0 || a.len_ == 0) return BigInt();
        BigInt p;
        p.reallocate(a.len_ + 1);
        long carry = 0;
        for (std::size_t i = 0; i < a.len_; ++i) {
            long tmp   = std::abs(a.val_[i]) * d + carry;
            carry      = tmp / 10;
            p.val_[i]  = tmp % 10;
        }
        if (carry) p.val_[a.len_] = carry;
        p.trim();
        return p;
    }

    // push one least‑significant digit at *front* (i.e. multiply by 10 and add d)
    static void pushFrontDigit(BigInt &r, long d) { // d in 0‑9, r positive
        if (r.len_ == 0 && d == 0) return;          // still zero
        long *buf = new long[r.len_ + 1];
        buf[0] = d;
        for (std::size_t i = 0; i < r.len_; ++i) buf[i + 1] = r.val_[i];
        delete[] r.val_;
        r.val_ = buf;
        r.len_ += 1;
    }

    // core long‑division: returns quotient, remainder via refs
    static void divmod(const BigInt &u, const BigInt &v, BigInt &q, BigInt &r) {
        if (v.len_ == 0) throw std::runtime_error("BigInt division by zero");
        // Signs
        int sign_u = (u.len_ && u.val_[u.len_ - 1] < 0) ? -1 : 1;
        int sign_v = (v.len_ && v.val_[v.len_ - 1] < 0) ? -1 : 1;
        int sign_q = sign_u * sign_v;
        int sign_r = sign_u;

        // Make positive copies
        BigInt a = u; for (std::size_t i = 0; i < a.len_; ++i) a.val_[i] = std::abs(a.val_[i]);
        BigInt b = v; for (std::size_t i = 0; i < b.len_; ++i) b.val_[i] = std::abs(b.val_[i]);

        if (compareAbs(a, b) < 0) { // |u| < |v| → q = 0, r = u
            q = 0;
            r = u;
            return;
        }

        q.reallocate(a.len_); // maximum length = dividend length
        r = BigInt();         // remainder starts at 0

        for (std::size_t idx = a.len_; idx-- > 0;) {
            pushFrontDigit(r, a.val_[idx]);
            // find digit d (0‑9) such that b*d ≤ r < b*(d+1)
            int d = 0;
            if (compareAbs(r, b) >= 0) {
                // upper bound 9, so linear search OK
                for (d = 9; d > 0; --d) {
                    BigInt prod = mulDigit(b, d);
                    if (compareAbs(prod, r) <= 0) break;
                }
                r = absSub(r, mulDigit(b, d));
            }
            q.val_[idx] = d;
        }
        q.trim();
        r.trim();
        // apply signs
        for (std::size_t i = 0; i < q.len_; ++i) q.val_[i] *= sign_q;
        for (std::size_t i = 0; i < r.len_; ++i) r.val_[i] *= sign_r;
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
        trim();
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
        sum.trim();
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
        prod.trim();
        return prod;
    }

    /*–––––––– helpers for mixing with built‑ins/strings ––––––––*/
    friend BigInt operator+(const BigInt &a, long b)        { return a + BigInt(b); }
    friend BigInt operator+(const BigInt &a, const char *s) { return a + BigInt(std::string(s)); }
    friend BigInt operator*(const BigInt &a, long b)        { return a * BigInt(b); }
    friend BigInt operator*(const BigInt &a, const char *s) { return a * BigInt(std::string(s)); }
    friend BigInt operator/(const BigInt &a, const BigInt &b) {
        BigInt q, r; divmod(a, b, q, r); return q;
    }
    friend BigInt operator%(const BigInt &a, const BigInt &b) {
        BigInt q, r; divmod(a, b, q, r); return r;
    }
};

int main() {
    BigInt a = 1;
    for (int i = 1; i <= 1000; ++i) a = a * i;   // 1000!
    std::cout << a << '\n';
}

// int main() {
//     BigInt n("34567890987654567890987654567890987654456783151513");
//     BigInt d("31523524365326353215213553113267235");
//     std::cout << "n / d = " << (n / d) << '\n';
//     std::cout << "n % d = " << (n % d) << '\n';
// }