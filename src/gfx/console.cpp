#include "gfx/console.hpp"
#include "gfx/font8x16.hpp"

namespace gfx {

  static constexpr size_t PADDING_X = 16;
  static constexpr size_t PADDING_Y = 16;
  static constexpr size_t TAB_WIDTH = 4;

  void Console::putPixel(size_t column, size_t row, uint32_t color) noexcept {
    if (column < mWidth && row < mHeight) {
      mBuffer[row * mStride + column] = color;
    }
  }

  void Console::drawGlyph(char ch, size_t column, size_t row) noexcept {
    uint8_t code = static_cast<uint8_t>(ch);
    if (code < font::FIRST_CHAR || code > font::LAST_CHAR) {
      code = '?';
    }

    const uint8_t* glyph = font::GLYPHS[code - font::FIRST_CHAR];
    const size_t startX = PADDING_X + column * font::GLYPH_WIDTH;
    const size_t startY = PADDING_Y + row * font::GLYPH_HEIGHT;

    for (size_t row = 0; row < font::GLYPH_HEIGHT; ++row) {
      const uint8_t rowBits = glyph[row];
      for (size_t column = 0; column < font::GLYPH_WIDTH; ++column) {
        const bool isSet = (rowBits & (0x80 >> column)) != 0;
        putPixel(startX + column, startY + row, isSet ? mFgColor : mBgColor);
      }
    }
  }

  void Console::scrollUp() noexcept {
    const size_t topRow = PADDING_Y;
    const size_t bottomRow = PADDING_Y + mMaxRows * font::GLYPH_HEIGHT;
    const size_t lineHeight = font::GLYPH_HEIGHT;

    // Shift text rows upward by one character height
    for (size_t row = topRow; row + lineHeight < bottomRow; ++row) {
      for (size_t column = PADDING_X; column < PADDING_X + mMaxColumns * font::GLYPH_WIDTH; ++column) {
        mBuffer[row * mStride + column] = mBuffer[(row + lineHeight) * mStride + column];
      }
    }

    // Clear the last line with background color
    for (size_t row = bottomRow - lineHeight; row < bottomRow; ++row) {
      for (size_t column = PADDING_X; column < PADDING_X + mMaxColumns * font::GLYPH_WIDTH; ++column) {
        mBuffer[row * mStride + column] = mBgColor;
      }
    }
  }

  void Console::newLine() noexcept {
    mCursorColumn = 0;
    ++mCursorRow;
    if (mCursorRow >= mMaxRows) {
      scrollUp();
      mCursorRow = mMaxRows - 1;
    }
  }

  bool Console::initialize(limine_framebuffer* frameBuffer,
                           uint32_t foregroundColor,
                           uint32_t backgroundColor) noexcept {
    if (frameBuffer == nullptr || frameBuffer->address == nullptr || frameBuffer->bpp != 32) {
      return false;
    }

    mBuffer = static_cast<uint32_t*>(frameBuffer->address);
    mWidth = frameBuffer->width;
    mHeight = frameBuffer->height;
    mStride = frameBuffer->pitch / 4;

    if (mWidth <= PADDING_X * 2 + font::GLYPH_WIDTH || mHeight <= PADDING_Y * 2 + font::GLYPH_HEIGHT) {
      mBuffer = nullptr;
      return false;
    }

    mMaxColumns = (mWidth - PADDING_X * 2) / font::GLYPH_WIDTH;
    mMaxRows = (mHeight - PADDING_Y * 2) / font::GLYPH_HEIGHT;
    mFgColor = foregroundColor;
    mBgColor = backgroundColor;

    clear();
    return true;
  }

  bool Console::isReady() noexcept {
    return mBuffer != nullptr;
  }

  void Console::clear() noexcept {
    if (!isReady()) {
      return;
    }

    for (size_t row = 0; row < mHeight; ++row) {
      for (size_t column = 0; column < mWidth; ++column) {
        mBuffer[row * mStride + column] = mBgColor;
      }
    }

    mCursorColumn = 0;
    mCursorRow = 0;
  }

  void Console::setColors(uint32_t foregroundColor, uint32_t backgroundColor) noexcept {
    mFgColor = foregroundColor;
    mBgColor = backgroundColor;
  }

  void Console::writeChar(char ch) noexcept {
    if (!isReady()) {
      return;
    }

    switch (ch) {
      case '\n':
        newLine();
        return;

      case '\r':
        mCursorColumn = 0;
        return;

      case '\t': {
        size_t nextStop = (mCursorColumn / TAB_WIDTH + 1) * TAB_WIDTH;
        if (nextStop > mMaxColumns) {
          nextStop = mMaxColumns;
        }
        while (mCursorColumn < nextStop) {
          writeChar(' ');
          if (mCursorColumn == 0) {
            break;
          }
        }
        return;
      }

      default: {
        drawGlyph(ch, mCursorColumn, mCursorRow);
        ++mCursorColumn;
        if (mCursorColumn >= mMaxColumns) {
          newLine();
        }
        return;
      }
    }
  }

  uint32_t Console::foregroundColor() noexcept {
    return mFgColor;
  }

  uint32_t Console::backgroundColor() noexcept {
    return mBgColor;
  }

}  // namespace gfx
