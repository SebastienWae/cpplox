#ifndef CPPLOX_BOX_HPP
#define CPPLOX_BOX_HPP

#include <memory>

template <typename T>
class Box {
  std::unique_ptr<T> m_impl;

 public:
  Box(T&& obj) : m_impl(std::make_unique<T>(std::move(obj))) {}
  Box(T const& obj) : m_impl(std::make_unique<T>(obj)) {}

  Box(Box const& other) : Box(*other.m_impl) {}

  auto operator=(Box const& other) -> Box& {
    if (&other == this) {
      return *this;
    }

    *m_impl = *other.m_impl;
    return *this;
  }

  ~Box() = default;

  Box(Box&&) noexcept = default;
  auto operator=(Box&&) noexcept -> Box& = default;

  auto operator*() -> T& { return *m_impl; }
  auto operator*() const -> T const& { return *m_impl; }

  auto operator->() -> T* { return m_impl.get(); }
  auto operator->() const -> T const* { return m_impl.get(); }
};

#endif /* CPPLOX_BOX_HPP */
