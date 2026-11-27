#pragma once

#include <stdint.h>
#include "lib/traits.hpp"
#include "lib/text_output.hpp"
#include "gfx/console.hpp"

namespace lib {

  struct Hex {
    uint64_t value;
  };

  class LogStream : private TextOutput<LogStream> {
   public:
    LogStream() noexcept = default;
    LogStream(const char* tag, uint32_t tagColor) noexcept;
    ~LogStream() noexcept;

    LogStream(const LogStream&) = delete;
    LogStream& operator=(const LogStream&) = delete;

    LogStream(LogStream&&) = delete;
    LogStream& operator=(LogStream&&) = delete;

    LogStream& operator<<(const char* str) noexcept {
      write(str);
      return *this;
    }

    LogStream& operator<<(char ch) noexcept {
      if (isReady()) {
        writeChar(ch);
      }
      return *this;
    }

    LogStream& operator<<(bool value) noexcept {
      write(value ? "true" : "false");
      return *this;
    }

    LogStream& operator<<(Hex hex) noexcept {
      writeHex(hex.value);
      return *this;
    }

    template <UnsignedIntegral T>
    LogStream& operator<<(T value) noexcept {
      writeDec(static_cast<uint64_t>(value));
      return *this;
    }

    template <SignedIntegral T>
    LogStream& operator<<(T value) noexcept {
      if (value < 0) {
        if (isReady()) {
          writeChar('-');
        }
        // Unsigned subtraction avoids signed integer overflow UB on INT64_MIN
        writeDec(0ULL - static_cast<uint64_t>(value));
      } else {
        writeDec(static_cast<uint64_t>(value));
      }

      return *this;
    }

   private:
    friend class TextOutput<LogStream>;

    static bool isReady() noexcept;
    static void writeChar(char ch) noexcept;
  };

  class Log {
   public:
    static LogStream info() noexcept {
      return { "[INFO]", gfx::Console::COLOR_CYAN };
    }

    static LogStream ok() noexcept {
      return { "[OK  ]", gfx::Console::COLOR_GREEN };
    }

    static LogStream warning() noexcept {
      return { "[WARN]", gfx::Console::COLOR_YELLOW };
    }

    static LogStream error() noexcept {
      return { "[ERR ]", gfx::Console::COLOR_RED };
    }
  };

}  // namespace lib
