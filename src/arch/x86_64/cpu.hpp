#pragma once

namespace arch::cpu {

  inline void disableInterrupts() noexcept {
    asm volatile("cli" ::: "memory");
  }

  inline void enableInterrupts() noexcept {
    asm volatile("sti" ::: "memory");
  }

  inline void waitForInterrupt() noexcept {
    asm volatile("hlt");
  }

  inline void pause() noexcept {
    asm volatile("pause");
  }

  [[noreturn]] inline void halt() noexcept {
    disableInterrupts();
    while (true) {
      waitForInterrupt();
    }
  }

}  // namespace arch::cpu
