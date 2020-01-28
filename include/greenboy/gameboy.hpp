#pragma once
#include <memory>

#include "cpu.hpp"

namespace greenboy {
/**
 * the interface between user and core emulator.
 */
class Gameboy {
  std::unique_ptr<CPU> m_cpu;

public:
  /**
   * The main constructor for the Gameboy.
   * It provides opertunities for dependency injection.
   * 
   * \param cpu A pointer to an instance of the CPU interface.
   */
  explicit Gameboy(std::unique_ptr<CPU> cpu);

  /**
   * Advance the emulation by the smallest step possible.
   * 
   * \callgraph
   */
  void step();
};
} // namespace greenboy
