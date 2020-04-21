#pragma once

#include "byte_access.hpp"
#include "word_access.hpp"

#include <memory>

namespace greenboy::data_access {
/**
 * @brief
 *
 */
class IndirectByte : public ByteAccess {
  std::shared_ptr<WordAccess> m_pointer;

public:
  /**
   * @brief
   *
   * @param pointer
   */
  explicit IndirectByte(std::shared_ptr<WordAccess> pointer);

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
  cycles access_time() const override;

  /**
   * @brief
   *
   * @param pointer
   * @return
   */
  static std::shared_ptr<IndirectByte>
  from(std::shared_ptr<WordAccess> pointer);
};
} // namespace greenboy::data_access
