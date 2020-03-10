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

  /**
   * @brief holds the register values for the CPU
   */
  struct RegisterSet {
    /**
     * @brief The program counter or instruction pointer.
     * Points to the address of the next instruction to be executed
     *
     */
    word pc;

    /**
     * @brief The stack pointer.
     * Points to the top of the stack;
     */
    word sp;

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
  };
};
} // namespace greenboy
