#pragma once

#include "word_access.hpp"

#include <memory>

namespace greenboy::data_access {
class IncrementingWord final : public WordAccess {
  std::shared_ptr<WordAccess> m_inner;

public:
  explicit IncrementingWord(std::shared_ptr<WordAccess> inner);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  static std::shared_ptr<IncrementingWord>
  from(std::shared_ptr<WordAccess> inner);
};
} // namespace greenboy::data_access
