#ifndef CPPLOX_ENUMNAME_HPP
#define CPPLOX_ENUMNAME_HPP

#include <array>
#include <string_view>

#ifndef ENUM_MAX_VALUE
#define ENUM_MAX_VALUE 127
#endif

#ifndef ENUM_MIN_VALUE
#define ENUM_MIN_VALUE 0
#endif

namespace Enums {
constexpr auto is_pretty(char ch) noexcept {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

constexpr auto pretty_name(std::string_view sv) noexcept {
  for (std::size_t n = sv.size() - 1; n > 0; --n) {
    if (!is_pretty(sv[n])) {
      sv.remove_prefix(n + 1);
      break;
    }
  }
  return sv;
}

template <typename E, E V> constexpr auto n() noexcept {
#if defined(__GNUC__) || defined(__clang__)
  return pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
  return pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#endif
}

template <typename E, E V> constexpr auto is_valid() {
  constexpr E v = static_cast<E>(V);
  return !n<E, V>().empty();
}

template <typename E> constexpr auto ualue(std::size_t v) {
  return static_cast<E>(ENUM_MIN_VALUE + v);
}

template <std::size_t N> constexpr auto count_values(const bool (&valid)[N]) {
  std::size_t count = 0;
  for (std::size_t n = 0; n < N; ++n) {
    if (valid[n]) {
      ++count;
    }
  }
  return count;
}

template <typename E, std::size_t... I>
constexpr auto values(std::index_sequence<I...>) noexcept {
  constexpr bool valid[sizeof...(I)] = {is_valid<E, ualue<E>(I)>()...};

  constexpr auto num_valid = count_values(valid);
  static_assert(num_valid > 0, "no support for empty enums");

  std::array<E, num_valid> values = {};
  for (std::size_t offset = 0, n = 0; n < num_valid; ++offset) {
    if (valid[offset]) {
      values[n] = ualue<E>(offset);
      ++n;
    }
  }

  return values;
}

template <typename E> constexpr auto values() noexcept {
  constexpr auto enum_size = ENUM_MAX_VALUE - ENUM_MIN_VALUE + 1;
  return values<E>(std::make_index_sequence<enum_size>({}));
}

template <typename E> inline constexpr auto values_v = values<E>();

template <typename E, E V> constexpr auto enum_name() {
  constexpr auto name = n<E, V>();
  return name;
}

template <typename E, E V>
inline constexpr auto enum_name_v = enum_name<E, V>();

template <typename E, std::size_t... I>
constexpr auto entries(std::index_sequence<I...>) noexcept {
  return std::array<std::pair<E, std::string_view>, sizeof...(I)>{
      {{values_v<E>[I], enum_name_v<E, values_v<E>[I]>}...}};
}

template <typename E>
inline constexpr auto entries_v =
    entries<E>(std::make_index_sequence<values_v<E>.size()>());

template <typename E>
constexpr auto enum_to_string(E value) -> std::string_view {
  for (const auto &[key, name] : entries_v<E>) {
    if (value == key) {
      return name;
    }
  }
  return {};
}

} // namespace Enums

#endif /* CPPLOX_ENUMNAME_HPP */
