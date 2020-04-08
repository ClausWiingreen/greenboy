#pragma once
#include <functional>
#include <memory>
#include <utility>

#include "cpu.hpp"
#include "memory_bus.hpp"
#include "timing.hpp"

namespace greenboy {
/**
 * @brief an interface for instructions which are performed by the CPU
 *
 */
class Instruction {
public:
  Instruction() = default;
  Instruction(const Instruction &) = delete;
  Instruction(Instruction &&) = delete;

  virtual ~Instruction() = default;

  Instruction &operator=(const Instruction &) = delete;
  Instruction &operator=(Instruction &&) = delete;

  /**
   * @brief executes the instruction
   *
   * @param registers the non memorymapped registers which may be modified by
   * the instruction
   * @param memory the memory map which may be written to or read from
   * @return the number of cpu cycles it took to execute this instruction
   */
  virtual cycles execute(CPU::RegisterSet &registers,
                         MemoryBus & /* memory */) const = 0;
};

namespace instructions {

// Access

class ByteAccess {
public:
  ByteAccess() = default;
  ByteAccess(const ByteAccess &) = delete;
  ByteAccess(ByteAccess &&) = delete;

  virtual ~ByteAccess() = default;

  ByteAccess &operator=(const ByteAccess &) = delete;
  ByteAccess &operator=(ByteAccess &&) = delete;

  virtual byte read(CPU::RegisterSet &registers, MemoryBus &memory) const = 0;
  virtual void write(CPU::RegisterSet &registers, MemoryBus &memory,
                     byte value) = 0;
};

class WordAccess {
public:
  WordAccess() = default;
  WordAccess(const WordAccess &) = delete;
  WordAccess(WordAccess &&) = delete;

  virtual ~WordAccess() = default;

  WordAccess &operator=(const WordAccess &) = delete;
  WordAccess &operator=(WordAccess &&) = delete;

  virtual word read(CPU::RegisterSet &registers, MemoryBus &memory) const = 0;
  virtual void write(CPU::RegisterSet &registers, MemoryBus &memory,
                     word value) = 0;
};

class ConstantByteAccess : public ByteAccess {
  byte m_value;

public:
  explicit ConstantByteAccess(byte value) : m_value(value) {}

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class ByteRegisterAccess : public ByteAccess {
  CPU::R8 m_reg;

public:
  explicit ByteRegisterAccess(CPU::R8 reg) : m_reg(reg) {}

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class ImmediateByteAccess : public ByteAccess {
public:
  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class IndirectByteAccess : public ByteAccess {
  std::shared_ptr<WordAccess> m_pointer;

public:
  explicit IndirectByteAccess(std::shared_ptr<WordAccess> pointer)
      : m_pointer(std::move(pointer)) {}

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class IndirectAndIncrementByteAccess : public ByteAccess {
  IndirectByteAccess m_inner;
  std::shared_ptr<WordAccess> m_pointer;

public:
  explicit IndirectAndIncrementByteAccess(
      const std::shared_ptr<WordAccess> &pointer)
      : m_inner(pointer), m_pointer(pointer) {}

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class IndirectAndDecrementByteAccess : public ByteAccess {
  IndirectByteAccess m_inner;
  std::shared_ptr<WordAccess> m_pointer;

public:
  explicit IndirectAndDecrementByteAccess(
      const std::shared_ptr<WordAccess> &pointer)
      : m_inner(pointer), m_pointer(pointer) {}

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class WordRegisterAccess : public WordAccess {
  CPU::R16 m_reg;

public:
  explicit WordRegisterAccess(CPU::R16 reg) : m_reg(reg) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
};

class ImmediateWordAccess : public WordAccess {
  ImmediateByteAccess m_access;

public:
  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
};

class DoubleByteWordAccess : public WordAccess {
  std::shared_ptr<ByteAccess> m_high;
  std::shared_ptr<ByteAccess> m_low;

public:
  DoubleByteWordAccess(std::shared_ptr<ByteAccess> high,
                       std::shared_ptr<ByteAccess> low)
      : m_high(std::move(high)), m_low(std::move(low)) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
};

// Instructions

class ByteLoad : public Instruction {
  std::shared_ptr<ByteAccess> m_destination;
  std::shared_ptr<const ByteAccess> m_source;

public:
  ByteLoad(std::shared_ptr<ByteAccess> dest,
           std::shared_ptr<const ByteAccess> src);

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

} // namespace instructions
} // namespace greenboy
