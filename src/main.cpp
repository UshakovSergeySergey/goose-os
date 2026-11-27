#include <stddef.h>
#include <stdint.h>
#include <limine.h>

#include "arch/x86_64/cpu.hpp"
#include "drivers/serial.hpp"
#include "gfx/console.hpp"
#include "lib/log.hpp"

namespace {

  [[gnu::used, gnu::section(".limine_requests")]]
  volatile uint64_t limineBaseRevision[] = LIMINE_BASE_REVISION(6);

  [[gnu::used, gnu::section(".limine_requests")]]
  volatile limine_framebuffer_request frameBufferRequest = { .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
                                                             .revision = 0,
                                                             .response = nullptr };

  [[gnu::used, gnu::section(".limine_requests_start")]]
  volatile uint64_t limineRequestsStartMarker[] = LIMINE_REQUESTS_START_MARKER;

  [[gnu::used, gnu::section(".limine_requests_end")]]
  volatile uint64_t limineRequestsEndMarker[] = LIMINE_REQUESTS_END_MARKER;

}  // namespace

extern "C" void kmain() {
  drivers::Serial::initialize();

  if (!LIMINE_BASE_REVISION_SUPPORTED(limineBaseRevision)) {
    lib::Log::error("Bootloader does not support Limine base revision 6!");
    arch::cpu::halt();
  }

  if (frameBufferRequest.response == nullptr || frameBufferRequest.response->framebuffer_count < 1 ||
      frameBufferRequest.response->framebuffers == nullptr) {
    lib::Log::error("Bootloader did not provide a framebuffer!");
    arch::cpu::halt();
  }

  limine_framebuffer* frameBuffer = frameBufferRequest.response->framebuffers[0];
  if (!gfx::Console::initialize(frameBuffer)) {
    lib::Log::error("Unsupported framebuffer format (expected 32 bpp)!");
    arch::cpu::halt();
  }

  lib::Log::writeLine("========================================");
  lib::Log::writeLine("     GooseOS x86_64 Kernel (v0.3.0)     ");
  lib::Log::writeLine("========================================");

  lib::Log::boot("Kernel initialized in 64-bit Long Mode");
  lib::Log::info("UART 16550 (COM1): 115200 baud, 8N1");

  lib::Log::info("Framebuffer: ");
  lib::Log::writeDec(frameBuffer->width);
  lib::Log::write("x");
  lib::Log::writeDec(frameBuffer->height);
  lib::Log::write(" @ ");
  lib::Log::writeHex(reinterpret_cast<uintptr_t>(frameBuffer->address));
  lib::Log::writeLine("");

  lib::Log::ok("Console subsystem ready. Halting CPU.");

  arch::cpu::halt();
}
