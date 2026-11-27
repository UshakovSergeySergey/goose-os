#pragma once

#include <stdint.h>

namespace lib {

  // Output primitives (safely no-op if underlying output system is not ready)

  template <typename Derived>
  class TextOutput {
   public:
    static void write(const char* str) noexcept {
      if (!Derived::isReady() || str == nullptr) {
        return;
      }

      for (const char* it = str; *it != '\0'; ++it) {
        Derived::writeChar(*it);
      }
    }

    static void writeLine(const char* str) noexcept {
      if (!Derived::isReady()) {
        return;
      }
      write(str);
      Derived::writeChar('\n');
    }

    static void writeHex(uint64_t value) noexcept {
      if (!Derived::isReady()) {
        return;
      }

      constexpr char hexDigits[] = "0123456789ABCDEF";
      write("0x");
      for (int shift = 60; shift >= 0; shift -= 4) {
        Derived::writeChar(hexDigits[(value >> shift) & 0x0F]);
      }
    }

    static void writeDec(uint64_t value) noexcept {
      if (!Derived::isReady()) {
        return;
      }

      if (value == 0) {
        Derived::writeChar('0');
        return;
      }

      char digits[20];  // Max uint64_t is 18,446,744,073,709,551,615 (20 digits)
      int index = 0;
      while (value > 0) {
        digits[index++] = static_cast<char>('0' + (value % 10));
        value /= 10;
      }
      while (index > 0) {
        Derived::writeChar(digits[--index]);
      }
    }
  };

}  // namespace lib
