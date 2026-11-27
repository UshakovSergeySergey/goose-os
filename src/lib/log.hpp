#pragma once

#include <stdint.h>

namespace lib {

  class Log {
   public:
    static void boot(const char* message) noexcept;
    static void info(const char* message) noexcept;
    static void ok(const char* message) noexcept;
    static void warning(const char* message) noexcept;
    static void error(const char* message) noexcept;

    static void write(const char* str) noexcept;
    static void writeLine(const char* str = "") noexcept;
    static void writeHex(uint64_t value) noexcept;
    static void writeDec(uint64_t value) noexcept;

   private:
    static void printTag(const char* tag, uint32_t tagColor, const char* message) noexcept;
  };

}  // namespace lib
