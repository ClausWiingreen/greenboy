#pragma once

#include "word_access.hpp"

namespace greenboy::data_access {
/**
 * @brief
 *
 */
class WordRegister : public WordAccess {
  CPU::R16 m_reg;

public:
  /**
   * @brief
   *
   * @param reg
   */
  explicit WordRegister(CPU::R16 reg);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;

  /**
   * @brief
   *
   * @return
   */
  static std::shared_ptr<WordRegister> bc();
  /**
   * @brief
   *
   * @return
   */
  static std::shared_ptr<WordRegister> de();
  /**
   * @brief
   *
   * @return
   */
  static std::shared_ptr<WordRegister> hl();
  /**
   * @brief
   *
   * @return
   */
  static std::shared_ptr<WordRegister> sp();
};
} // namespace greenboy::data_access
