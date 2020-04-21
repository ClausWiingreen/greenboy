#pragma once

#include <functional>
#include <stdexcept>
#include <variant>

#include "timing.hpp"
#include "types.hpp"

namespace greenboy {
/**
 * CPU interface for Gameboy emulation.
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

  /**
   * @brief This enum is the ways that the registers can be accesses on the CPU
   * as 8 bit values.
   */
  enum class R8 { B, C, D, E, H, L, A };
  /**
   * @brief This enum is the ways that the registers can be accesses on the CPU
   * as 16 bit values.
   */
  enum class R16 { BC, DE, HL, SP, PC, AF };

  /**
   * @brief the flags used internally by the cpu
   *
   */
  struct Flags {

    /**
     * @brief the zero flag. Indicates whether the result of the last operation
     * resulted in zero.
     */
    bool zero = false;
    /**
     * @brief the negate flag. Indicates whether the last operation was a
     * negation operation.
     */
    bool negate = false;
    /**
     * @brief the half carry flag. Indicates whether the last operation caused
     * overflow between the 3rd and 4th bit to occur.
     */
    bool half_carry = false;
    /**
     * @brief the carry flag. Indicates whether the last operation caused an
     * overflow.
     */
    bool carry = false;
    /**
     * @brief Default constructor.
     * Constructs false initialize set of Flags.
     *
     */
    constexpr Flags() noexcept = default;

    /**
     * @brief Constructs the flags from the byte representation in <c>value</c>.
     * Only the 4 most significant bits are used.
     *
     * @param value the byte representation of the flags
     */
    explicit constexpr Flags(byte value) noexcept {
      zero = (value.value() & (1u << 7u)) != 0;
      negate = (value.value() & (1u << 6u)) != 0;
      half_carry = (value.value() & (1u << 5u)) != 0;
      carry = (value.value() & (1u << 4u)) != 0;
    }
    /**
     * @brief Copy constructor. Constructs the Flags with the copy of the
     * contents of <c>other</c>.
     *
     * @param other another Flags to use as a source to initialize the Flags
     * with
     */
    Flags(const Flags &other) noexcept = default;
    /**
     * @brief Move constructor. Constructs the Flags with the contents of
     * <c>other</c> using move semantics.
     * <c>other</c> is left in a valid, but unspecified state.
     *
     * @param other another Flags to use as a source to initialize the Flags
     * with
     */
    Flags(Flags &&other) noexcept = default;
    /**
     * @brief destroys the Flags
     *
     */
    ~Flags() noexcept = default;

    /**
     * @brief Replaces the contents with a copy of <c>f</c>.
     * If <c>f</c> and <c>*this</c> is the same, this function has no effect.
     *
     * @param f Flags to be used to initialize the Flags with
     * @return <c>*thiz</c>
     */
    Flags &operator=(const Flags &f) noexcept = default;

    /**
     * @brief Replaces the contents with those of <c>f</c> using move semantics.
     * <c>f</c> is in a valid but unspecified state afterwards.
     *
     * @param f Flags to be used to initialize the Flags with
     * @return <c>*thiz</c>
     */
    Flags &operator=(Flags &&f) noexcept = default;

    /**
     * @brief Cast to byte operator
     *
     * @return A byte representation of the flags
     */
    [[nodiscard]] constexpr explicit operator byte() const noexcept {
      return byte{static_cast<uint8_t>(
          (zero ? 0x80u : 0u) | (negate ? 0x40u : 0u) |
          (half_carry ? 0x20u : 0u) | (carry ? 0x10u : 0u))};
    }

    /**
     * @brief Checks if the flags are the same
     *
     * @param other Flags which content to compare with.
     * @return <c>true</c> when none of the flags differ; otherwise <c>false</c>
     */
    [[nodiscard]] constexpr bool operator==(const Flags &other) const noexcept {
      return zero == other.zero && negate == other.negate &&
             half_carry == other.half_carry && carry == other.carry;
    }

    /**
     * @brief Checks if the flags are different
     *
     * @param other Flags which content to compare with.
     * @return <c>true</c> when at least one of the flags differ; otherwise
     * <c>false</c>
     */
    [[nodiscard]] constexpr bool operator!=(const Flags &other) const noexcept {
      return zero != other.zero || negate != other.negate ||
             half_carry != other.half_carry || carry != other.carry;
    }
  };

  /**
   * @brief The class RegisterSet holds the register values for the CPU
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
