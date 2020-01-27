#pragma once

#include "timing.hpp"

namespace greenboy {
/** 
 * a well defined interface for CPU emulation.
 */
class CPU {
public:
  virtual ~CPU() = default;

  /**Updates the state of the CPU.
   *
   * @return the amount of clock cycles spent
   */
  virtual cycles update() = 0;
};
} // namespace greenboy
