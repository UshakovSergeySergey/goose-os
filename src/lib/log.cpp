#include "lib/log.hpp"
#include "drivers/serial.hpp"
#include "gfx/console.hpp"

namespace lib {

  void Log::boot(const char* message) noexcept {
    printTag("[BOOT]", gfx::Console::COLOR_CYAN, message);
  }

  void Log::info(const char* message) noexcept {
    printTag("[INFO]", gfx::Console::COLOR_CYAN, message);
  }

  void Log::ok(const char* message) noexcept {
    printTag("[OK]  ", gfx::Console::COLOR_GREEN, message);
  }

  void Log::warning(const char* message) noexcept {
    printTag("[WARN]", gfx::Console::COLOR_YELLOW, message);
  }

  void Log::error(const char* message) noexcept {
    printTag("[ERR] ", gfx::Console::COLOR_RED, message);
  }

  void Log::write(const char* str) noexcept {
    drivers::Serial::write(str);
    gfx::Console::write(str);
  }

  void Log::writeLine(const char* str) noexcept {
    drivers::Serial::writeLine(str);
    gfx::Console::writeLine(str);
  }

  void Log::writeHex(uint64_t value) noexcept {
    drivers::Serial::writeHex(value);
    gfx::Console::writeHex(value);
  }

  void Log::writeDec(uint64_t value) noexcept {
    drivers::Serial::writeDec(value);
    gfx::Console::writeDec(value);
  }

  void Log::printTag(const char* tag, uint32_t tagColor, const char* message) noexcept {
    drivers::Serial::write(tag);
    drivers::Serial::write(" ");
    drivers::Serial::write("message");

    gfx::Console::setColors(tagColor, gfx::Console::COLOR_SLATE);
    gfx::Console::write(tag);
    gfx::Console::setColors(gfx::Console::COLOR_WHITE, gfx::Console::COLOR_SLATE);
    gfx::Console::write(" ");
    gfx::Console::write(message);
  }

}  // namespace lib
