#pragma once

#include "byte_access.hpp"

#include <memory>

namespace greenboy::data_access {
class ByteRegister final : public ByteAccess {
  CPU::R8 m_reg;

public:
  explicit ByteRegister(CPU::R8 reg) noexcept;

  [[nodiscard]] byte read(CPU::RegisterSet &registers,
                          MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;

  [[nodiscard]] static std::shared_ptr<ByteRegister> b() noexcept;
  [[nodiscard]] static std::shared_ptr<ByteRegister> c() noexcept;
  [[nodiscard]] static std::shared_ptr<ByteRegister> d() noexcept;
  [[nodiscard]] static std::shared_ptr<ByteRegister> h() noexcept;
  [[nodiscard]] static std::shared_ptr<ByteRegister> a() noexcept;
};
} // namespace greenboy::data_access
