#pragma once

#include "byte_access.hpp"

#include <memory>

namespace greenboy::data_access {
class ConstantByte final : public ByteAccess {
  byte m_value;

public:
  explicit ConstantByte(byte value);

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;

  static std::shared_ptr<ConstantByte> from(byte value);
};
} // namespace greenboy::data_access
