#pragma once

#include "word_access.hpp"

#include <memory>

namespace greenboy::data_access {
/**
 * @brief
 *
 */
class IndirectWord : public WordAccess {
  std::shared_ptr<WordAccess> m_pointer;

public:
  /**
   * @brief Construct a new Indirect Word object
   *
   * @param pointer
   */
  explicit IndirectWord(std::shared_ptr<WordAccess> pointer);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override;

  /**
   * @brief
   *
   * @param pointer
   * @return std::shared_ptr<ConstantByte>
   */
  static std::shared_ptr<IndirectWord>
  from(std::shared_ptr<WordAccess> pointer);
};
} // namespace greenboy::data_access