#pragma once

#include "word_access.hpp"

#include <memory>

namespace greenboy::data_access {
class ByteAccess;

class OffsatWord final : public WordAccess {
  std::shared_ptr<WordAccess> m_access;
  std::shared_ptr<ByteAccess> m_offset;

public:
  OffsatWord(std::shared_ptr<WordAccess> access,
             std::shared_ptr<ByteAccess> offset);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override;

  static std::shared_ptr<OffsatWord> from(std::shared_ptr<WordAccess> access,
                                          std::shared_ptr<ByteAccess> offset);
};
} // namespace greenboy::data_access
