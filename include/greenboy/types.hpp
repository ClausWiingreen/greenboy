#pragma once

#include <cstdint>
#include <type_traits>

namespace greenboy {

class word;

/**
 * @brief container for a single 8 bit value
 *
 */
class byte {
  uint8_t m_value = 0;

public:
  constexpr byte() noexcept = default;

  /**
   * @brief constructs a new byte object which is initialized to the given value
   *
   * @tparam IntType the type of the parameter, must be an intergal type
   * @param value the 8-bit value stored in the byte
   */
  template <typename IntType>
  explicit constexpr byte(IntType value) noexcept
      : m_value{static_cast<uint8_t>(value)} {
    static_assert(std::is_integral<IntType>::value,
                  "IntType must be an integral type");
  }
  constexpr byte(const byte &) noexcept = default;
  constexpr byte(byte &&) noexcept = default;

  ~byte() noexcept = default;

  byte &operator=(const byte &) noexcept = default;
  byte &operator=(byte &&) noexcept = default;

  constexpr bool operator==(const byte &other) const {
    return other.m_value == m_value;
  }

  constexpr bool operator!=(const byte &other) const {
    return other.m_value != m_value;
  }

  constexpr byte &operator|=(const byte &other) {
    m_value |= other.m_value;
    return *this;
  }

  constexpr byte operator|(const byte &other) const {
    return byte{m_value | other.m_value};
  }

  constexpr byte &operator&=(const byte &other) {
    m_value &= other.m_value;
    return *this;
  }

  constexpr byte operator&(const byte &other) const {
    return byte{m_value & other.m_value};
  }

  constexpr byte &operator^=(const byte &other) {
    m_value ^= other.m_value;
    return *this;
  }

  constexpr byte operator^(const byte &other) const {
    return byte{m_value ^ other.m_value};
  }

  constexpr byte &operator<<=(unsigned value) {
    m_value = static_cast<uint8_t>(m_value << value);
    return *this;
  }

  constexpr byte operator<<(unsigned value) const {
    return byte{m_value << value};
  }

  constexpr byte &operator>>=(const uint8_t value) {
    m_value = static_cast<uint8_t>(m_value >> value);
    return *this;
  }

  constexpr byte operator>>(const uint8_t value) const {
    return byte{m_value << value};
  }

  constexpr byte operator~() const { return byte{~m_value}; }

  friend class greenboy::word;
};

static_assert(sizeof(byte) == 1, "Bytes are required to only be 1 byte long");

/**
 * @brief container for a 16 bit value
 *
 */
class word {
  uint16_t m_value = 0;

public:
  word() noexcept = default;
  word(const word &) noexcept = default;
  word(word &&) noexcept = default;
  constexpr word(byte low, byte high) noexcept
      : word{low.m_value + (high.m_value << 8u)} {}
  explicit constexpr word(int value) noexcept
      : m_value{static_cast<uint16_t>(value)} {}

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

  constexpr byte high() const { return static_cast<byte>(m_value >> 8u); }

  constexpr byte low() const { return static_cast<byte>(m_value); }
};
static_assert(sizeof(word) == 2, "Words are required to be 2 bytes long");
} // namespace greenboy
