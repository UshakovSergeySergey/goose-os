#include <stddef.h>
#include <stdint.h>
#include <limine.h>

#include "arch/x86_64/cpu.hpp"
#include "drivers/serial.hpp"

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
  if (!LIMINE_BASE_REVISION_SUPPORTED(limineBaseRevision)) {
    arch::cpu::halt();
  }

  drivers::serial::init();
  drivers::serial::writeLine("[BOOT] GooseOS v0.2.0 kernel started");

  if (frameBufferRequest.response == nullptr || frameBufferRequest.response->framebuffer_count < 1 ||
      frameBufferRequest.response->framebuffers == nullptr) {
    drivers::serial::writeLine("[ERROR] No framebuffer available");
    arch::cpu::halt();
  }

  limine_framebuffer* frameBuffer = frameBufferRequest.response->framebuffers[0];
  if (frameBuffer == nullptr || frameBuffer->address == nullptr) {
    drivers::serial::writeLine("[ERROR] Invalid framebuffer pointer");
    arch::cpu::halt();
  }

  drivers::serial::write("[INFO] Framebuffer: ");
  drivers::serial::writeDec(frameBuffer->width);
  drivers::serial::write("x");
  drivers::serial::writeDec(frameBuffer->height);
  drivers::serial::write(" @ ");
  drivers::serial::writeHex(reinterpret_cast<uintptr_t>(frameBuffer->address));
  drivers::serial::writeLine("");

  auto* pixels = static_cast<uint32_t*>(frameBuffer->address);
  const size_t stride = frameBuffer->pitch / 4;

  for (size_t row = 0; row < frameBuffer->height; ++row) {
    for (size_t column = 0; column < frameBuffer->width; ++column) {
      pixels[row * stride + column] = 0x0000FF00;
    }
  }

  constexpr size_t squareSize = 120;
  const size_t startX = (frameBuffer->width > squareSize) ? (frameBuffer->width - squareSize) / 2 : 0;
  const size_t startY = (frameBuffer->height > squareSize) ? (frameBuffer->height - squareSize) / 2 : 0;
  const size_t endX = (startX + squareSize < frameBuffer->width) ? (startX + squareSize) : frameBuffer->width;
  const size_t endY = (startY + squareSize < frameBuffer->height) ? (startY + squareSize) : frameBuffer->height;

  for (size_t row = startY; row < endY; ++row) {
    for (size_t column = startX; column < endX; ++column) {
      pixels[row * stride + column] = 0x00FFFFFF;
    }
  }

  drivers::serial::writeLine("[OK] Screen initialized. Halting CPU.");

  arch::cpu::halt();
}
