#pragma once

#include "timing.hpp"
#include "types.hpp"

namespace greenboy {
class CPU {
public:
  CPU() noexcept = default;
  CPU(const CPU &other) = delete;
  CPU(CPU &&) = delete;

  virtual ~CPU() noexcept = default;

  CPU &operator=(const CPU &) = delete;
  CPU &operator=(CPU &&) = delete;

  virtual cycles update() = 0;

  enum class R8 { B, C, D, E, H, L, A };
  enum class R16 { BC, DE, HL, SP, PC, AF };

  struct Flags {
    bool zero = false;
    bool negate = false;
    bool half_carry = false;
    bool carry = false;

    constexpr Flags() noexcept = default;

    explicit constexpr Flags(byte value) noexcept {
      zero = (value & byte(1u << 7u)) != byte{};
      negate = (value & byte(1u << 6u)) != byte{};
      half_carry = (value & byte(1u << 5u)) != byte{};
      carry = (value & byte(1u << 4u)) != byte{};
    }

    [[nodiscard]] constexpr explicit operator byte() const noexcept {
      return byte{static_cast<uint8_t>(
          (zero ? 0x80u : 0u) | (negate ? 0x40u : 0u) |
          (half_carry ? 0x20u : 0u) | (carry ? 0x10u : 0u))};
    }
  };

  struct RegisterSet {
    word pc{};
    word sp{};
    byte b{};
    byte c{};
    byte d{};
    byte e{};
    byte h{};
    byte l{};
    byte a{};
    Flags f{};
  };
};

[[nodiscard]] constexpr bool operator==(const CPU::Flags &lhs,
                                        const CPU::Flags &rhs) noexcept {
  return lhs.zero == rhs.zero && lhs.negate == rhs.negate &&
         lhs.half_carry == rhs.half_carry && lhs.carry == rhs.carry;
}

[[nodiscard]] constexpr bool operator!=(const CPU::Flags &lhs,
                                        const CPU::Flags &rhs) noexcept {
  return lhs.zero != rhs.zero || lhs.negate != rhs.negate ||
         lhs.half_carry != rhs.half_carry || lhs.carry != rhs.carry;
}

[[nodiscard]] constexpr bool operator==(const CPU::RegisterSet &lhs,
                                        const CPU::RegisterSet &rhs) noexcept {
  return lhs.b == rhs.b && lhs.c == rhs.c && lhs.d == rhs.d && lhs.e == rhs.e &&
         lhs.h == rhs.h && lhs.l == rhs.l && lhs.a == rhs.a && lhs.f == rhs.f;
}

[[nodiscard]] constexpr bool operator!=(const CPU::RegisterSet &lhs,
                                        const CPU::RegisterSet &rhs) noexcept {
  return lhs.b != rhs.b || lhs.c != rhs.c || lhs.d != rhs.d || lhs.e != rhs.e ||
         lhs.h != rhs.h || lhs.l != rhs.l || lhs.a != rhs.a || lhs.f != rhs.f;
}
} // namespace greenboy
