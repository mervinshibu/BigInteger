# BigInt  

A **lightweight, self‑contained, arbitrary‑precision integer** class written in modern C++17. It’s a cleaned‑up rewrite of an old undergraduate project, now featuring Rule‑of‑Five memory safety and support for addition, multiplication, **integer division, and modulo**.

---

## ✨ Features

| ✔                        | Capability                                                            | Notes                                               |
| ------------------------ | --------------------------------------------------------------------- | --------------------------------------------------- |
| Safe resource management | Follows the **Rule of Five**, zero memory leaks                       | Raw `new[]`/`delete[]` kept for educational clarity |
| Unlimited precision      | Digits stored little‑endian, base‑10                                  | Easy to read / debug in a console                   |
| Arithmetic               | `+  *  /  %` between `BigInt`s or with `long`/string literals         | Division by zero throws `std::runtime_error`        |
| Header‑only option       | Just drop `BigInt.hpp` into your project (rename the .cpp if desired) |                                                     |
| Zero dependencies        | Uses only the C++ standard library                                    | No GMP / Boost MP                                   |

---

## 🔧 Building & Running

```bash
# Clone
$ git clone https://github.com/<your‑handle>/BigInt.git && cd BigInt

# Compile the demo (GCC/Clang/MSVC all fine)
$ g++ -std=c++17 -O2 BigInt.cpp -o bigint_demo

# Run
$ ./bigint_demo
```

You can also add the file to an existing CMake project:

```cmake
add_library(BigInt OBJECT BigInt.cpp)
```

---

## 🛠️ API Sketch

```cpp
#include "BigInt.hpp"   // or BigInt.cpp if you prefer the single‑file variant

BigInt a = 42;                  // implicit from long
BigInt b("123456789012345");   // from string literal

BigInt sum  = a + b;
BigInt prod = a * b;
BigInt q    = b / 97;
BigInt r    = b % 97;
std::cout << q << " remainder " << r << "\n";
```

| Method / Operator       | Complexity             | Comment               |
| ----------------------- | ---------------------- | --------------------- |
| `operator+`             | Θ( max(N,M) )          | Linear                |
| `operator*`             | Θ(N·M)                 | Grade‑school multiply |
| `operator/` `operator%` | Θ(N·M + N²) ≈ Θ(N²)`*` | Long division         |

`*` Worst case when operands have similar length.

---

## 📈 Performance

For a few hundred to a few thousand digits this implementation is more than fast enough for competitive programming or teaching demos. If you need millions of digits, swap in an FFT‑based multiply/divide.

---

## 🤝 Contributing

1. Fork → Branch → PR.
2. Please include unit tests (see `/tests`).
3. Follow the style existing in `BigInt.cpp` (spaces, not tabs; 100 col).

Ideas welcome: Karatsuba / FFT multiplication, larger internal base (e.g. base 10⁹ limbs), JSON/stream serialisation, CI benchmarks.