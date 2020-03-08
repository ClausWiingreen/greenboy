#pragma once

#include <cstdint>

namespace greenboy {
enum class byte : uint8_t {};
enum class word : uint16_t {};

constexpr word to_word(int value) { return static_cast<word>(value); }
constexpr word &operator++(word &value) {
  return value = to_word(static_cast<uint16_t>(value) + 1);
}
constexpr word operator++(word &value, int) {
  auto copy = value;
  value = to_word(static_cast<uint16_t>(value) + 1);
  return copy;
}
constexpr word &operator--(word &value) {
  return value = to_word(static_cast<uint16_t>(value) - 1);
}
constexpr word operator--(word &value, int) {
  auto copy = value;
  value = to_word(static_cast<uint16_t>(value) - 1);
  return copy;
}

} // namespace greenboy
