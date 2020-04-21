#pragma once
#include <memory>

#include "greenboy/data_access.hpp"

namespace greenboy::instructions {
/**
 * @brief
 *
 */
class ByteLoad : public Instruction {
  std::shared_ptr<data_access::ByteAccess> m_destination;
  std::shared_ptr<const data_access::ByteAccess> m_source;

public:
  /**
   * @brief Construct a new Byte Load object
   *
   * @param dest
   * @param src
   */
  ByteLoad(std::shared_ptr<data_access::ByteAccess> dest,
           std::shared_ptr<const data_access::ByteAccess> src);

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief
 *
 */
class WordLoad : public Instruction {
  std::shared_ptr<data_access::WordAccess> m_destination;
  std::shared_ptr<const data_access::WordAccess> m_source;

public:
  /**
   * @brief Construct a new Word Load object
   *
   * @param dest
   * @param src
   */
  WordLoad(std::shared_ptr<data_access::WordAccess> dest,
           std::shared_ptr<const data_access::WordAccess> src);
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};
} // namespace greenboy::instructions
