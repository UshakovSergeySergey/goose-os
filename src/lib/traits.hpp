#pragma once

namespace lib {

  template <typename T, typename U>
  inline constexpr bool isSame = false;

  template <typename T>
  inline constexpr bool isSame<T, T> = true;

  template <typename T, typename U>
  concept SameAs = isSame<T, U>;

  template <typename T>
  struct RemoveCv {
    using Type = T;
  };

  template <typename T>
  struct RemoveCv<const T> {
    using Type = T;
  };

  template <typename T>
  struct RemoveCv<volatile T> {
    using Type = T;
  };

  template <typename T>
  struct RemoveCv<const volatile T> {
    using Type = T;
  };

  template <typename T>
  using RemoveCvT = typename RemoveCv<T>::Type;

  template <typename T>
  concept SignedIntegral = SameAs<RemoveCvT<T>, signed char> || SameAs<RemoveCvT<T>, short> ||
                           SameAs<RemoveCvT<T>, int> || SameAs<RemoveCvT<T>, long> || SameAs<RemoveCvT<T>, long long>;

  template <typename T>
  concept UnsignedIntegral = SameAs<RemoveCvT<T>, unsigned char> || SameAs<RemoveCvT<T>, unsigned short> ||
                             SameAs<RemoveCvT<T>, unsigned int> || SameAs<RemoveCvT<T>, unsigned long> ||
                             SameAs<RemoveCvT<T>, unsigned long long>;

}  // namespace lib
