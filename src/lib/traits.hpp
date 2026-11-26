#pragma once

namespace lib {

  template <typename T, typename U>
  inline constexpr bool isSame = false;

  template <typename T>
  inline constexpr bool isSame<T, T> = true;

  template <typename T, typename U>
  concept SameAs = isSame<T, U>;

}  // namespace lib
