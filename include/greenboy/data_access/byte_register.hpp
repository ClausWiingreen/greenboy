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
  [[nodiscard]] static std::shared_ptr<ByteRegister> e() noexcept;
  [[nodiscard]] static std::shared_ptr<ByteRegister> h() noexcept;
  [[nodiscard]] static std::shared_ptr<ByteRegister> l() noexcept;
  [[nodiscard]] static std::shared_ptr<ByteRegister> a() noexcept;
};

inline std::shared_ptr<ByteRegister> ByteRegister::b() noexcept {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::B);
  return instance;
}
inline std::shared_ptr<ByteRegister> ByteRegister::c() noexcept {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::C);
  return instance;
}
inline std::shared_ptr<ByteRegister> ByteRegister::d() noexcept {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::D);
  return instance;
}
inline std::shared_ptr<ByteRegister> ByteRegister::e() noexcept {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::E);
  return instance;
}
inline std::shared_ptr<ByteRegister> ByteRegister::h() noexcept {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::H);
  return instance;
}
inline std::shared_ptr<ByteRegister> ByteRegister::l() noexcept {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::L);
  return instance;
}
inline std::shared_ptr<ByteRegister> ByteRegister::a() noexcept {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::A);
  return instance;
}
} // namespace greenboy::data_access
