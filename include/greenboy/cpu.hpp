#pragma once

#include "timing.hpp"

namespace greenboy {
/** 
 * a well defined interface for CPU emulation.
 */
class CPU {
public:
  CPU() = default;
  CPU(const CPU&) = delete;
  CPU(CPU&&) = delete;
  
  virtual ~CPU() = default;

  CPU& operator=(const CPU&) = delete;
  CPU& operator=(CPU&&) = delete;

  /**
   * Updates the state of the CPU.
   *
   * @return the amount of clock cycles spent
   */
  virtual cycles update() = 0;
};
} // namespace greenboy
