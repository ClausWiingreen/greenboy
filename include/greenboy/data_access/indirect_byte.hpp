#pragma once

#include "byte_access.hpp"

#include <memory>

namespace greenboy::data_access {
class WordAccess;

class IndirectByte final : public ByteAccess {
  std::shared_ptr<WordAccess> m_pointer;

public:
  explicit IndirectByte(std::shared_ptr<WordAccess> pointer);

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
  cycles access_time() const noexcept override;

  static std::shared_ptr<IndirectByte>
  from(std::shared_ptr<WordAccess> pointer);
};
} // namespace greenboy::data_access
