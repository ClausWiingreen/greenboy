#pragma once

#include "./byte_access.hpp"

namespace greenboy::data_access {
/**
 * @brief
 *
 */
class ConstantByte : public ByteAccess {
  byte m_value;

public:
  explicit ConstantByte(byte value);

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;

  static std::shared_ptr<ConstantByte> from(byte value);
};
} // namespace greenboy::data_access
