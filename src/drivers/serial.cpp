#include "drivers/serial.hpp"
#include "arch/x86_64/cpu.hpp"
#include "arch/x86_64/port.hpp"

namespace drivers {

  namespace {

    // UART 16550 register offsets relative to base port
    constexpr uint16_t DATA_REG = 0;         // Data register (R/W) or Divisor Latch Low (DLAB=1)
    constexpr uint16_t INT_ENABLE_REG = 1;   // Interrupt Enable (R/W) or Divisor Latch High (DLAB=1)
    constexpr uint16_t FIFO_CTRL_REG = 2;    // FIFO Control Register (W)
    constexpr uint16_t LINE_CTRL_REG = 3;    // Line Control Register (R/W)
    constexpr uint16_t MODEM_CTRL_REG = 4;   // Modem Control Register (R/W)
    constexpr uint16_t LINE_STATUS_REG = 5;  // Line Status Register (R)

    constexpr uint8_t LINE_STATUS_TX_EMPTY = 1 << 5;  // Transmitter holding register is empty

  }  // namespace

  bool Serial::initialize(Port port) noexcept {
    mActivePort = static_cast<uint16_t>(port);

    arch::port::write<uint8_t>(mActivePort + INT_ENABLE_REG, 0x00);  // Disable all UART interrupts
    arch::port::write<uint8_t>(mActivePort + LINE_CTRL_REG, 0x80);   // Enable DLAB (set baud rate divisor)
    arch::port::write<uint8_t>(mActivePort + DATA_REG, 0x01);        // Set divisor to 1 (low byte) -> 115200 baud
    arch::port::write<uint8_t>(mActivePort + INT_ENABLE_REG, 0x00);  // Set divisor to 0 (high byte, DLAB=1)
    arch::port::write<uint8_t>(mActivePort + LINE_CTRL_REG,
                               0x03);  // Disable DLAB; 8 bits, no parity, one stop bit (8N1)
    arch::port::write<uint8_t>(mActivePort + FIFO_CTRL_REG, 0xC7);   // Enable FIFO, clear queues, 14-byte threshold
    arch::port::write<uint8_t>(mActivePort + MODEM_CTRL_REG, 0x0B);  // Enable DTR, RTS and OUT2 (IRQ gate)

    // Loopback test to verify that a real/emulated UART chip is actually present
    arch::port::write<uint8_t>(mActivePort + MODEM_CTRL_REG, 0x1E);  // Set in loopback mode
    arch::port::write<uint8_t>(mActivePort + DATA_REG, 0xAE);        // Send test byte 0xAE
    if (arch::port::read<uint8_t>(mActivePort + DATA_REG) != 0xAE) {
      mIsAvailable = false;
      return false;
    }

    // Switch back to normal operation mode
    arch::port::write<uint8_t>(mActivePort + MODEM_CTRL_REG, 0x0F);
    mIsAvailable = true;
    return true;
  }

  bool Serial::isReady() noexcept {
    return mIsAvailable;
  }

  void Serial::writeChar(char ch) noexcept {
    if (!isReady()) {
      return;
    }

    if (ch == '\n') {
      writeChar('\r');
    }

    while ((arch::port::read<uint8_t>(mActivePort + LINE_STATUS_REG) & LINE_STATUS_TX_EMPTY) == 0) {
      arch::cpu::pause();
    }

    arch::port::write<uint8_t>(mActivePort + DATA_REG, static_cast<uint8_t>(ch));
  }

}  // namespace drivers
