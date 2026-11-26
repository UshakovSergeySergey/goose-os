#pragma once

#include <stdint.h>
#include "lib/traits.hpp"

namespace arch::port {

  template <typename T>
  concept PortValue = lib::SameAs<T, uint8_t> || lib::SameAs<T, uint16_t> || lib::SameAs<T, uint32_t>;

  template <PortValue T>
  inline void write(uint16_t port, T value) noexcept {
    if constexpr (lib::SameAs<T, uint8_t>) {
      asm volatile("outb %0, %1" : : "a"(value), "Nd"(port) : "memory");
    } else if constexpr (lib::SameAs<T, uint16_t>) {
      asm volatile("outw %0, %1" : : "a"(value), "Nd"(port) : "memory");
    } else if constexpr (lib::SameAs<T, uint32_t>) {
      asm volatile("outl %0, %1" : : "a"(value), "Nd"(port) : "memory");
    }
  }

  template <PortValue T>
  inline T read(uint16_t port) noexcept {
    T value{};
    if constexpr (lib::SameAs<T, uint8_t>) {
      asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port) : "memory");
    } else if constexpr (lib::SameAs<T, uint16_t>) {
      asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port) : "memory");
    } else if constexpr (lib::SameAs<T, uint32_t>) {
      asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port) : "memory");
    }
    return value;
  }

  inline void wait() noexcept {
    write<uint8_t>(0x80, 0);
  }

}  // namespace arch::port
