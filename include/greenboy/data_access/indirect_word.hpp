#pragma once

#include "word_access.hpp"

#include <memory>

namespace greenboy::data_access {
class IndirectWord final : public WordAccess {
  std::shared_ptr<WordAccess> m_pointer;

public:
  explicit IndirectWord(std::shared_ptr<WordAccess> pointer);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override;

  static std::shared_ptr<IndirectWord>
  from(std::shared_ptr<WordAccess> pointer);
};
} // namespace greenboy::data_access