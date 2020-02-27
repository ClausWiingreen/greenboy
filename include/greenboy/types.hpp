#pragma once

#include <cstdint>

namespace greenboy {
enum class byte : uint8_t {};
enum class word : uint16_t {};

constexpr word toWord(int value){ return static_cast<word>(value); }

} // namespace greenboy
