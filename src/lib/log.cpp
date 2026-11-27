#include "lib/log.hpp"
#include "drivers/serial.hpp"
#include "gfx/console.hpp"

namespace lib {

  LogStream::LogStream(const char* tag, uint32_t tagColor) noexcept {
    if (!isReady()) {
      return;
    }

    drivers::Serial::write(tag);
    drivers::Serial::write(" ");

    const uint32_t previousFgColor = gfx::Console::foregroundColor();
    const uint32_t previousBgColor = gfx::Console::backgroundColor();

    gfx::Console::setColors(tagColor, previousBgColor);
    gfx::Console::write(tag);
    gfx::Console::setColors(previousFgColor, previousBgColor);
    gfx::Console::write(" ");
  }

  LogStream::~LogStream() {
    if (isReady()) {
      writeChar('\n');
    }
  }

  bool LogStream::isReady() noexcept {
    return drivers::Serial::isReady() || gfx::Console::isReady();
  }

  void LogStream::writeChar(char ch) noexcept {
    drivers::Serial::writeChar(ch);
    gfx::Console::writeChar(ch);
  }

}  // namespace lib
