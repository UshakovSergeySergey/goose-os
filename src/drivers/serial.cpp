#include "drivers/serial.hpp"
#include "arch/x86_64/cpu.hpp"
#include "arch/x86_64/port.hpp"

namespace drivers::serial {

  namespace {

    // UART 16550 register offsets relative to base port
    constexpr uint16_t DATA_REG = 0;         // Data register (R/W) or Divisor Latch Low (DLAB=1)
    constexpr uint16_t INT_ENABLE_REG = 1;   // Interrupt Enable (R/W) or Divisor Latch High (DLAB=1)
    constexpr uint16_t FIFO_CTRL_REG = 2;    // FIFO Control Register (W)
    constexpr uint16_t LINE_CTRL_REG = 3;    // Line Control Register (R/W)
    constexpr uint16_t MODEM_CTRL_REG = 4;   // Modem Control Register (R/W)
    constexpr uint16_t LINE_STATUS_REG = 5;  // Line Status Register (R)

    constexpr uint8_t LINE_STATUS_TX_EMPTY = 1 << 5;  // Transmitter holding register is empty

    uint16_t activePort = static_cast<uint16_t>(Port::Com1);
    bool isAvailable = false;

  }  // namespace

  bool init(Port port) noexcept {
    activePort = static_cast<uint16_t>(port);

    arch::port::write<uint8_t>(activePort + INT_ENABLE_REG, 0x00);  // Disable all UART interrupts
    arch::port::write<uint8_t>(activePort + LINE_CTRL_REG, 0x80);   // Enable DLAB (set baud rate divisor)
    arch::port::write<uint8_t>(activePort + DATA_REG, 0x01);        // Set divisor to 1 (low byte) -> 115200 baud
    arch::port::write<uint8_t>(activePort + INT_ENABLE_REG, 0x00);  // Set divisor to 0 (high byte, DLAB=1)
    arch::port::write<uint8_t>(activePort + LINE_CTRL_REG,
                               0x03);  // Disable DLAB; 8 bits, no parity, one stop bit (8N1)
    arch::port::write<uint8_t>(activePort + FIFO_CTRL_REG, 0xC7);   // Enable FIFO, clear queues, 14-byte threshold
    arch::port::write<uint8_t>(activePort + MODEM_CTRL_REG, 0x0B);  // Enable DTR, RTS and OUT2 (IRQ gate)

    // Loopback test to verify that a real/emulated UART chip is actually present
    arch::port::write<uint8_t>(activePort + MODEM_CTRL_REG, 0x1E);  // Set in loopback mode
    arch::port::write<uint8_t>(activePort + DATA_REG, 0xAE);        // Send test byte 0xAE
    if (arch::port::read<uint8_t>(activePort + DATA_REG) != 0xAE) {
      isAvailable = false;
      return false;
    }

    // Switch back to normal operation mode
    arch::port::write<uint8_t>(activePort + MODEM_CTRL_REG, 0x0F);
    isAvailable = true;
    return true;
  }

  void writeChar(char ch) noexcept {
    if (!isAvailable) {
      return;
    }

    while ((arch::port::read<uint8_t>(activePort + LINE_STATUS_REG) & LINE_STATUS_TX_EMPTY) == 0) {
      arch::cpu::pause();
    }

    arch::port::write<uint8_t>(activePort + DATA_REG, static_cast<uint8_t>(ch));
  }

  void write(const char* str) noexcept {
    if (!isAvailable || str == nullptr) {
      return;
    }

    for (const char* it = str; *it != '\0'; ++it) {
      if (*it == '\n') {
        writeChar('\r');
      }
      writeChar(*it);
    }
  }

  void writeLine(const char* str) noexcept {
    write(str);
    write("\n");
  }

  void writeHex(uint64_t value) noexcept {
    if (!isAvailable) {
      return;
    }

    constexpr char hexDigits[] = "0123456789ABCDEF";
    write("0x");
    for (int shift = 60; shift >= 0; shift -= 4) {
      writeChar(hexDigits[(value >> shift) & 0x0F]);
    }
  }

  void writeDec(uint64_t value) noexcept {
    if (!isAvailable) {
      return;
    }

    if (value == 0) {
      writeChar('0');
      return;
    }

    char buffer[20];  // Max uint64_t is 18,446,744,073,709,551,615 (20 digits)
    int index = 0;
    while (value > 0) {
      buffer[index++] = static_cast<char>('0' + (value % 10));
      value /= 10;
    }
    while (index > 0) {
      writeChar(buffer[--index]);
    }
  }

}  // namespace drivers::serial
