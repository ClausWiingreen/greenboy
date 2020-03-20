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
  /**
   * @brief zero initialized default constuctor
   *
   */
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

  /**
   * @brief default copy constructor
   *
   */
  constexpr byte(const byte &) noexcept = default;

  /**
   * @brief default move constructor
   *
   */
  constexpr byte(byte &&) noexcept = default;

  /**
   * @brief Destroy the byte object
   *
   */
  ~byte() noexcept = default;

  /**
   * @brief default copy assignment operation
   *
   * @return byte& instance of the object that is assigned to
   */
  byte &operator=(const byte &) noexcept = default;

  /**
   * @brief default move assignment operator
   *
   * @return byte& instance of the object that is assigned to
   */
  byte &operator=(byte &&) noexcept = default;

  /**
   * @brief compares the stored values in the bytes
   *
   * @param other the other byte to compare with
   * @return true if they are equal
   * @return false if they are not equal
   */
  constexpr bool operator==(const byte &other) const {
    return other.m_value == m_value;
  }

  /**
   * @brief compares the stored values in the bytes
   *
   * @param other the other byte to compare with
   * @return true if they are not equal
   * @return false if they are equal
   */
  constexpr bool operator!=(const byte &other) const {
    return other.m_value != m_value;
  }

  /**
   * @brief performs a bitwize or operation on this object and the other
   *
   * @param other the other byte involved in the or operation
   * @return byte& reference to this object after performing the or operation
   */
  constexpr byte &operator|=(const byte &other) {
    m_value |= other.m_value;
    return *this;
  }

  /**
   * @brief performs a bitwize or operation on between this byte and the other
   * byte
   *
   * @param other the other byte involved in the or operation
   * @return byte a new byte with the value of the or'ed bytes
   */
  constexpr byte operator|(const byte &other) const {
    return byte{m_value | other.m_value};
  }

  /**
   * @brief performs a bitwize and operation on this object and the other
   *
   * @param other the other byte involved in the and operation
   * @return byte& reference to this object after performing the and operation
   */
  constexpr byte &operator&=(const byte &other) {
    m_value &= other.m_value;
    return *this;
  }

  /**
   * @brief performs a bitwize and operation on between this byte and the other
   * byte
   *
   * @param other the other byte involved in the and operation
   * @return byte a new byte with the value of the and'ed bytes
   */
  constexpr byte operator&(const byte &other) const {
    return byte{m_value & other.m_value};
  }

  /**
   * @brief performs a bitwize xor operation on this object and the other
   *
   * @param other the other byte involved in the xor operation
   * @return byte& reference to this object after performing the xor operation
   */
  constexpr byte &operator^=(const byte &other) {
    m_value ^= other.m_value;
    return *this;
  }

  /**
   * @brief performs a bitwize xor operation on between this byte and the other
   * byte
   *
   * @param other the other byte involved in the xor operation
   * @return byte a new byte with the value of the xor'ed bytes
   */
  constexpr byte operator^(const byte &other) const {
    return byte{m_value ^ other.m_value};
  }

  /**
   * @brief shifts the bits to the left the number of spots indicated by value
   *
   * @param value the number of times to shift to the left
   * @return  byte& reference to this object after performing the shift
   */
  constexpr byte &operator<<=(unsigned value) {
    m_value = static_cast<uint8_t>(m_value << value);
    return *this;
  }

  /**
   * @brief shifts the bits to the left the number of spots indicated by value
   *
   * @param value the number of times to shift to the left
   * @return byte a new byte that contain the value after shifting the values
   */
  constexpr byte operator<<(unsigned value) const {
    return byte{m_value << value};
  }

  /**
   * @brief shifts the bits to the right the number of spots indicated by value
   *
   * @param value the number of times to shift to the right
   * @return  byte& reference to this object after performing the shift
   */
  constexpr byte &operator>>=(const uint8_t value) {
    m_value = static_cast<uint8_t>(m_value >> value);
    return *this;
  }

  /**
   * @brief shifts the bits to the right the number of spots indicated by value
   *
   * @param value the number of times to shift to the right
   * @return byte a new byte that contain the value after shifting the values
   */
  constexpr byte operator>>(const uint8_t value) const {
    return byte{m_value << value};
  }

  /**
   * @brief creates a byte with an inverted bit patteren compared to this byte
   *
   * @return byte with its bit patteren inverted
   */
  constexpr byte operator~() const { return byte{~m_value}; }

  /**
   * @brief prefix increment operator increments the byte and returns the value
   * after the increment
   *
   * @return byte the byte after incrementing it
   */
  constexpr byte &operator++() {
    m_value++;
    return *this;
  }

  /**
   * @brief postfix increment operator increments the byte and returns the value
   * before the increment
   *
   * @return const byte the byte before incrementing it
   */
  constexpr const byte operator++(int) {
    byte copy{*this};
    m_value++;
    return copy;
  }

  /**
   * @brief prefix decrement operator decrements the byte and returns the value
   * after the decrement
   *
   * @return byte the byte after decrementing it
   */
  constexpr byte &operator--() {
    m_value--;
    return *this;
  }

  /**
   * @brief postfix decrement operator decrements the byte and returns the value
   * before the decrement
   *
   * @return const byte the byte before decrementing it
   */
  constexpr const byte operator--(int) {
    byte copy{*this};
    m_value--;
    return copy;
  }

  /**
   * @brief give word access to the inner value for construction
   *
   */
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
  /**
   * @brief Construct a new word object
   *
   */
  word() noexcept = default;

  /**
   * @brief Construct a new word object
   *
   */
  word(const word &) noexcept = default;

  /**
   * @brief Construct a new word object
   *
   */
  word(word &&) noexcept = default;

  /**
   * @brief constructs a new word object which is initialized to the given value
   *
   * @tparam IntType the type of the parameter, must be an intergal type
   * @param value the 16-bit value to be stored in the byte
   */
  template <typename IntType>
  explicit constexpr word(IntType value) noexcept
      : m_value{static_cast<uint16_t>(value)} {
    static_assert(std::is_integral<IntType>::value,
                  "IntType must be an integral type");
  }

  /**
   * @brief Constructs a new word object from two bytes
   *
   * @param low the lower 8-bits of data
   * @param high the upper 8-bits of data
   */
  constexpr word(byte low, byte high) noexcept
      : word{low.m_value + (high.m_value << 8u)} {}

  /**
   * @brief Destroy the word object
   *
   */
  ~word() noexcept = default;

  /**
   * @brief default copy assignment operator
   *
   * @return word& instance of the object that is assigned to
   */
  word &operator=(const word &) = default;

  /**
   * @brief default move assignment operator
   *
   * @return word& instance of the object that is assigned to
   */
  word &operator=(word &&) = default;

  /**
   * @brief prefix increment operator increments the word and returns the value
   * after the increment
   *
   * @return const word the word after incrementing it
   */
  constexpr word &operator++() {
    m_value++;
    return *this;
  }

  /**
   * @brief postfix increment operator increments the word and returns the value
   * before the increment
   *
   * @return const word the word before incrementing it
   */
  constexpr const word operator++(int) {
    word copy{*this};
    m_value++;
    return copy;
  }

  /**
   * @brief prefix decrement operator decrements the word and returns the value
   * after the decrement
   *
   * @return const word the word after decrementing it
   */
  constexpr word &operator--() {
    m_value--;
    return *this;
  }

  /**
   * @brief postfix decrement operator decrements the word and returns the value
   * before the decrement
   *
   * @return const word the word before decrementing it
   */
  constexpr const word operator--(int) {
    word copy{*this};
    m_value--;
    return copy;
  }

  /**
   * @brief checks if the words are  equal
   *
   * @param other the word to compare with
   * @return true if the stored value are equal
   * @return false if the stored value aren't equal
   */
  constexpr bool operator==(const word &other) const {
    return other.m_value == m_value;
  }

  /**
   * @brief checks if the words are not equal
   *
   * @param other the word to compare with
   * @return true if the stored value aren't equal
   * @return false if the stored value are equal
   */
  constexpr bool operator!=(const word &other) const {
    return other.m_value != m_value;
  }

  /**
   * @brief retreives the upper 8 bits and creates a byte from it.
   *
   * @return byte the most significant byte in the word
   */
  constexpr byte high() const { return byte{m_value >> 8u}; }

  /**
   * @brief retreives the lower 8 bits and creates a byte from it.
   *
   * @return byte the least significant byte in the word
   */
  constexpr byte low() const { return byte{m_value}; }
};
static_assert(sizeof(word) == 2, "Words are required to be 2 bytes long");
} // namespace greenboy
