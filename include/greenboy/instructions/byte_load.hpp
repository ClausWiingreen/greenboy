#pragma once
#include <memory>

#include "greenboy/instruction.hpp"

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
} // namespace greenboy::instructions
