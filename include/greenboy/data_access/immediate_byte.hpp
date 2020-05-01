#pragma once

#include "byte_access.hpp"

#include <memory>

namespace greenboy::data_access {
class ImmediateByte final : public ByteAccess {
public:
  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
  cycles access_time() const noexcept override;

  static std::shared_ptr<ImmediateByte> instance();
};
} // namespace greenboy::data_access
