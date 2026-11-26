#include <stddef.h>
#include <stdint.h>
#include <limine.h>

namespace {

  [[gnu::used, gnu::section(".limine_requests")]]
  volatile uint64_t limineBaseRevision[] = LIMINE_BASE_REVISION(6);

  [[gnu::used, gnu::section(".limine_requests")]]
  volatile limine_framebuffer_request frameBufferRequest = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
    .response = nullptr
  };

  [[gnu::used, gnu::section(".limine_requests_start")]]
  volatile uint64_t limineRequestsStartMarker[] = LIMINE_REQUESTS_START_MARKER;

  [[gnu::used, gnu::section(".limine_requests_end")]]
  volatile uint64_t limineRequestsEndMarker[] = LIMINE_REQUESTS_END_MARKER;

}

namespace arch::cpu {

  inline void disableInterrupts() noexcept {
    asm volatile("cli" ::: "memory");
  }

  inline void waitForInterrupt() noexcept {
    asm volatile("hlt");
  }

  [[noreturn]] void halt() {
    disableInterrupts();
    while (true) {
      waitForInterrupt();
    }
  }

}

extern "C" void kmain () {
  if(!LIMINE_BASE_REVISION_SUPPORTED(limineBaseRevision) ||
    frameBufferRequest.response == nullptr ||
    frameBufferRequest.response->framebuffer_count < 1) {
    arch::cpu::halt();
  }

  limine_framebuffer* frameBuffer = frameBufferRequest.response->framebuffers[0];
  if (frameBuffer == nullptr) {
    arch::cpu::halt();
  }

  auto* pixels = static_cast<uint32_t*>(frameBuffer->address);
  if (pixels == nullptr) {
    arch::cpu::halt();
  }

  const size_t stride = frameBuffer->pitch / 4;
  for (size_t row = 0; row < frameBuffer->height; ++row) {
    for (size_t column = 0; column < frameBuffer->width; ++column) {
      pixels[row * stride + column] = 0x0000FF00;
    }
  }

  for (size_t row = 60; row < 180 && row < frameBuffer->height; ++row) {
    for (size_t column = 60; column < 180 && column < frameBuffer->width; ++column) {
      pixels[row * stride + column] = 0x00FFFFFF;
    }
  }

  arch::cpu::halt();
}
