#pragma once

#include "timing.hpp"
#include "types.hpp"

namespace greenboy {
/**
 * a well defined interface for CPU emulation.
 */
class CPU {
public:
  //! @cond Doxygen_Suppress
  CPU() = default;
  CPU(const CPU &other) = delete;
  CPU(CPU &&) = delete;

  virtual ~CPU() = default;

  CPU &operator=(const CPU &) = delete;
  CPU &operator=(CPU &&) = delete;
  //! @endcond

  /**
   * Updates the state of the CPU.
   *
   * @return the amount of clock cycles spent
   */
  virtual cycles update() = 0;

  enum class R8 { B, C, D, E, H, L };

  class RegisterPair {
    byte &m_high;
    byte &m_low;

  public:
    constexpr RegisterPair(byte &high, byte &low) : m_high(high), m_low(low) {}

    constexpr byte &high() { return m_high; }
    constexpr byte &low() { return m_low; }
    constexpr const byte &high() const { return m_high; }
    constexpr const byte &low() const { return m_low; }
  };

  enum class R16 { BC, DE, HL };

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
    byte f;

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

    constexpr byte &reference(R8 identifier) {
      switch (identifier) {
      case R8::B:
        return b;
      case R8::C:
        return c;
      case R8::D:
        return d;
      case R8::E:
        return e;
      case R8::H:
        return h;
      case R8::L:
        return l;
      default:
        throw std::runtime_error("Tried to access an unknown 8 bit register");
      }
    }

    constexpr RegisterPair reference(R16 identifier) {
      switch (identifier) {
      case R16::BC:
        return RegisterPair(b, c);
      case R16::DE:
        return RegisterPair(d, e);
      case R16::HL:
        return RegisterPair(h, l);
      default:
        throw std::runtime_error("Tried to access an unknown 16 bit register");
      }
    }
  };
};
} // namespace greenboy
