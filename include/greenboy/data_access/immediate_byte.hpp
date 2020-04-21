#pragma once

#include "byte_access.hpp"

namespace greenboy::data_access {
/**
 * @brief
 *
 */
class ImmediateByte : public ByteAccess {
public:
  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
  cycles access_time() const override;

  /**
   * @brief
   *
   * @return
   */
  static std::shared_ptr<ImmediateByte> instance();
};
} // namespace greenboy::data_access
