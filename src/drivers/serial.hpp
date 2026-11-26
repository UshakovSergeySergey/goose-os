#pragma once

#include <stdint.h>

namespace drivers::serial {

  // Standard x86 COM port base I/O addresses
  enum class Port : uint16_t {
    Com1 = 0x3F8,
    Com2 = 0x2F8,
    Com3 = 0x3E8,
    Com4 = 0x2E8,
  };

  // Initialize UART 16550 controller; returns false if hardware is absent (e.g. modern laptops)
  bool init(Port port = Port::Com1) noexcept;

  // Output primitives (safely no-op if serial hardware was not detected)
  void writeChar(char ch) noexcept;
  void write(const char* str) noexcept;
  void writeLine(const char* str) noexcept;
  void writeHex(uint64_t value) noexcept;
  void writeDec(uint64_t value) noexcept;

}  // namespace drivers::serial
