#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace greenboy {

using std::byte;
using std::to_integer;

using word = std::uint16_t;

constexpr word to_word(byte high, byte low) {
  return static_cast<word>((to_integer<unsigned>(high) << 8u) | to_integer<unsigned>(low));
}

constexpr byte high_byte(word w) { return static_cast<byte>(w >> 8u); }
constexpr byte low_byte(word w) { return static_cast<byte>(w); }
} // namespace greenboy
