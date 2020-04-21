#pragma once

#include "word_access.hpp"

#include <memory>

namespace greenboy::data_access {
/**
 * @brief
 *
 */
class DecrementingWord : public WordAccess {
  std::shared_ptr<WordAccess> m_inner;

public:
  /**
   * @brief
   *
   * @param inner
   */
  explicit DecrementingWord(std::shared_ptr<WordAccess> inner);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;

  /**
   * @brief
   *
   * @param inner
   * @return
   */
  static std::shared_ptr<DecrementingWord>
  from(std::shared_ptr<WordAccess> inner);
};
} // namespace greenboy::data_access
