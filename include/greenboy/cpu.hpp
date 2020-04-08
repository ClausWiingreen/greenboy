#pragma once

#include <functional>
#include <stdexcept>
#include <variant>

#include "timing.hpp"
#include "types.hpp"

namespace greenboy {
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

/**
 * a well defined interface for CPU emulation.
 */
class CPU {
public:
  CPU() = default;
  CPU(const CPU &other) = delete;
  CPU(CPU &&) = delete;

  virtual ~CPU() = default;

  CPU &operator=(const CPU &) = delete;
  CPU &operator=(CPU &&) = delete;

  /**
   * Updates the state of the CPU.
   *
   * @return the amount of clock cycles spent
   */
  virtual cycles update() = 0;

  enum class R8 { B, C, D, E, H, L, A };
  enum class R16 { BC, DE, HL, SP, PC, AF };

  struct Flags {
    bool zero;
    bool negate;
    bool half_carry;
    bool carry;

    Flags() = default;
    explicit Flags(byte value) {
      zero = (value.value() & 0x80u) != 0;
      negate = (value.value() & 0x40u) != 0;
      half_carry = (value.value() & 0x20u) != 0;
      carry = (value.value() & 0x10u) != 0;
    }
    Flags(const Flags &) = default;
    Flags(Flags &&) = default;

    ~Flags() = default;

    Flags &operator=(const Flags &) = default;
    Flags &operator=(Flags &&) = default;

    explicit operator byte() const {
      return byte{static_cast<uint8_t>(
          (zero ? 0x80u : 0u) | (negate ? 0x40u : 0u) |
          (half_carry ? 0x20u : 0u) | (carry ? 0x10u : 0u))};
    }

    constexpr bool operator==(const Flags &other) const {
      return zero == other.zero && negate == other.negate &&
             half_carry == other.half_carry && carry == other.carry;
    }

    constexpr bool operator!=(const Flags &other) const {
      return zero != other.zero || negate != other.negate ||
             half_carry != other.half_carry || carry != other.carry;
    }
  };

  /**
   * @brief holds the register values for the CPU
   */
  struct RegisterSet {
    /**
     * @brief The program counter or instruction pointer.
     * Points to the address of the next instruction to be executed
     *
     */
    word pc{0};

    /**
     * @brief The stack pointer.
     * Points to the top of the stack;
     */
    word sp{0};

    /**
     * @brief General purpose register b is commonly used as an 8-bit counter.
     * Can be used together with c to form 16 bit register bc
     */
    byte b;

    /**
     * @brief General purpose register c is commonly used when you want to
     * interface with hardware ports.
     * Can be used together with b to form 16 bit register bc
     */
    byte c;

    /**
     * @brief General purpose register d is not normally used in its 8-bit
     * form. Instead, it is used in conjuncture with e.
     * Can be used together with e to form 16 bit register de
     */
    byte d;

    /**
     * @brief General purpose register e is not normally used in its 8-bit
     * form. Instead, it is used in conjuncture with d.
     * Can be used together with d to form 16 bit register de
     */
    byte e;

    /**
     * @brief General purpose register h is not normally used in its 8-bit
     * form. Instead, it is used in conjuncture with l.
     * Can be used together with b to form register hl
     */
    byte h;

    /**
     * @brief General purpose register l is not normally used in its 8-bit
     * form. Instead, it is used in conjuncture with h.
     * Can be used together with b to form register hl
     */
    byte l;

    /**
     * @brief The accumulator.
     * The result of most arithmetic operations is performed with this
     * register either as the sole operand or as one of the two.
     * Results are also normally stored here.
     *
     */
    byte a;

    /**
     * @brief The flag register.
     * Consists of 4 flags that are set or reset according to results of
     * instruction execution.
     */
    Flags f;

    /**
     * @brief checks whether the current RegisterSet is equal to the other
     * RegisterSet
     *
     * @param other the other registers to compare with
     * @return true when all registers are equal
     * @return false when some registers are not equal
     */
    constexpr bool operator==(const RegisterSet &other) const {
      return b == other.b && c == other.c && d == other.d && e == other.e &&
             h == other.h && l == other.l && a == other.a && f == other.f;
    }

    /**
     * @brief checks whether the current RegisterSet is not equal to the other
     * RegisterSet
     *
     * @param other the other registers to compare with
     * @return true when some registers are not equal
     * @return false when all registers are equal
     */
    constexpr bool operator!=(const RegisterSet &other) const {
      return b != other.b || c != other.c || d != other.d || e != other.e ||
             h != other.h || l != other.l || a != other.a || f != other.f;
    }
  };
};
} // namespace greenboy
