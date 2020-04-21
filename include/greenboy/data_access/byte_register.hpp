#pragma once

#include "byte_access.hpp"

#include <memory>

namespace greenboy::data_access {
/**
 * @brief
 *
 */
class ByteRegister : public ByteAccess {
  CPU::R8 m_reg;

public:
  explicit ByteRegister(CPU::R8 reg);

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;

  static std::shared_ptr<ByteRegister> b();
  static std::shared_ptr<ByteRegister> c();
  static std::shared_ptr<ByteRegister> d();
  static std::shared_ptr<ByteRegister> h();
  static std::shared_ptr<ByteRegister> a();
};
} // namespace greenboy::data_access
