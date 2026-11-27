#pragma once

#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "lib/text_output.hpp"

namespace gfx {

  class Console : public lib::TextOutput<Console> {
   public:
    // Basic 32-bit XRGB color palette
    static inline constexpr uint32_t COLOR_BLACK = 0x00000000;
    static inline constexpr uint32_t COLOR_WHITE = 0x00FFFFFF;
    static inline constexpr uint32_t COLOR_SLATE = 0x001E293B;
    static inline constexpr uint32_t COLOR_GRAY = 0x0094A3B8;
    static inline constexpr uint32_t COLOR_GREEN = 0x0022C55E;
    static inline constexpr uint32_t COLOR_CYAN = 0x0038BDF8;
    static inline constexpr uint32_t COLOR_YELLOW = 0x00EAB308;
    static inline constexpr uint32_t COLOR_RED = 0x00EF4444;

    static bool initialize(limine_framebuffer* frameBuffer,
                           uint32_t foregroundColor = COLOR_WHITE,
                           uint32_t backgroundColor = COLOR_SLATE) noexcept;
    static bool isReady() noexcept;

    // Safely no-op if the framebuffer console was not initialized
    static void clear() noexcept;
    static void setColors(uint32_t foregroundColor, uint32_t backgroundColor) noexcept;
    static void writeChar(char ch) noexcept;

   private:
    static void putPixel(size_t column, size_t row, uint32_t color) noexcept;
    static void drawGlyph(char ch, size_t column, size_t row) noexcept;
    static void scrollUp() noexcept;
    static void newLine() noexcept;

    static inline uint32_t* mBuffer = nullptr;
    static inline size_t mWidth = 0;
    static inline size_t mHeight = 0;
    static inline size_t mStride = 0;  // Pixels per row (pitch / 4)
    static inline size_t mMaxColumns = 0;
    static inline size_t mMaxRows = 0;
    static inline size_t mCursorColumn = 0;
    static inline size_t mCursorRow = 0;
    static inline uint32_t mFgColor = COLOR_WHITE;
    static inline uint32_t mBgColor = COLOR_SLATE;
  };

}  // namespace gfx
