#pragma once

#include "word_access.hpp"

#include <memory>

namespace greenboy::data_access {
/**
 * @brief
 *
 */
class DelayedWordAccess : public WordAccess {
  std::shared_ptr<WordAccess> m_inner;

public:
  explicit DelayedWordAccess(std::shared_ptr<WordAccess> inner);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override;

  /**
   * @brief
   *
   * @param inner
   * @return std::shared_ptr<ConstantByte>
   */
  static std::shared_ptr<DelayedWordAccess>
  from(std::shared_ptr<WordAccess> inner);
};
} // namespace greenboy::data_access