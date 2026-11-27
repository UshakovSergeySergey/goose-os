#pragma once

#include <stdint.h>
#include "lib/text_output.hpp"

namespace drivers {

  class Serial : public lib::TextOutput<Serial> {
   public:
    // Standard x86 COM port base I/O addresses
    enum class Port : uint16_t {
      Com1 = 0x3F8,
      Com2 = 0x2F8,
      Com3 = 0x3E8,
      Com4 = 0x2E8,
    };

    // Initialize UART 16550 controller; returns false if hardware is absent (e.g. modern laptops)
    static bool initialize(Port port = Port::Com1) noexcept;
    static bool isReady() noexcept;

    // Safely no-op if the serial port was not initialized or is unavailable
    static void writeChar(char ch) noexcept;

   private:
    static inline uint16_t mActivePort = static_cast<uint16_t>(Serial::Port::Com1);
    static inline bool mIsAvailable = false;
  };

}  // namespace drivers
