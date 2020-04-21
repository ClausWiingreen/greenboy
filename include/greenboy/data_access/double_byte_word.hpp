#pragma once

#include "byte_access.hpp"
#include "word_access.hpp"

namespace greenboy::data_access {
/**
 * @brief
 *
 */
class DoubleByteWord : public WordAccess {
  std::shared_ptr<ByteAccess> m_high;
  std::shared_ptr<ByteAccess> m_low;

public:
  /**
   * @brief
   *
   * @param high
   * @param low
   */
  DoubleByteWord(std::shared_ptr<ByteAccess> high,
                 std::shared_ptr<ByteAccess> low);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override;

  /**
   * @brief
   *
   * @param high
   * @param low
   * @return std::shared_ptr<ConstantByte>
   */
  static std::shared_ptr<DoubleByteWord> from(std::shared_ptr<ByteAccess> high,
                                              std::shared_ptr<ByteAccess> low);
};
} // namespace greenboy::data_access
