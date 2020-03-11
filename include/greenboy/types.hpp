#pragma once

#include <cstdint>

namespace greenboy {
enum class byte : uint8_t {};
class word {
  uint16_t m_value = 0;

public:
  word() noexcept = default;
  word(const word &) noexcept = default;
  word(word &&) noexcept = default;
  constexpr word(byte low, byte high) noexcept
      : word{static_cast<int>(low) | static_cast<int>(high) << 8} {}
  explicit constexpr word(int value) noexcept {
    m_value = static_cast<uint16_t>(value);
  }

  ~word() noexcept = default;

  word &operator=(const word &) = default;
  word &operator=(word &&) = default;

  constexpr word &operator++() {
    m_value++;
    return *this;
  }

  constexpr const word operator++(int) {
    word copy{*this};
    m_value++;
    return copy;
  }

  constexpr word &operator--() {
    m_value--;
    return *this;
  }

  constexpr const word operator--(int) {
    word copy{*this};
    m_value--;
    return copy;
  }

  constexpr bool operator==(const word &other) const {
    return other.m_value == m_value;
  }

  constexpr bool operator!=(const word &other) const {
    return other.m_value != m_value;
  }

  constexpr byte high() const { return static_cast<byte>(m_value >> 8); }

  constexpr byte low() const { return static_cast<byte>(m_value); }
};
} // namespace greenboy
