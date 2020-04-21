#pragma once

#include "byte_access.hpp"
#include "word_access.hpp"

namespace greenboy::data_access {
/**
 * @brief
 *
 */
class OffsatWord : public WordAccess {
  std::shared_ptr<WordAccess> m_access;
  std::shared_ptr<ByteAccess> m_offset;

public:
  /**
   * @brief Construct a new Offsat Word object
   *
   * @param access
   * @param offset
   */
  OffsatWord(std::shared_ptr<WordAccess> access,
             std::shared_ptr<ByteAccess> offset);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override;

  /**
   * @brief
   *
   * @param access
   * @param offset
   * @return std::shared_ptr<ConstantByte>
   */
  static std::shared_ptr<OffsatWord> from(std::shared_ptr<WordAccess> access,
                                          std::shared_ptr<ByteAccess> offset);
};
} // namespace greenboy::data_access
