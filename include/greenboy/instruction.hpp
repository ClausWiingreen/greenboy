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
                         MemoryBus &memory) const = 0;
};

namespace data_access {

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
  virtual cycles access_time() const { return cycles{}; }
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
  virtual cycles access_time() const { return cycles{}; }
};

class ConstantByte : public ByteAccess {
  byte m_value;

public:
  explicit ConstantByte(byte value) : m_value(value) {}

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;

  static std::shared_ptr<ConstantByte> from(byte value);
};

class ByteRegister : public ByteAccess {
  CPU::R8 m_reg;

public:
  explicit ByteRegister(CPU::R8 reg) : m_reg(reg) {}

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;

  static std::shared_ptr<ByteRegister> b();
  static std::shared_ptr<ByteRegister> c();
  static std::shared_ptr<ByteRegister> d();
  static std::shared_ptr<ByteRegister> h();
  static std::shared_ptr<ByteRegister> a();
};

class ImmediateByte : public ByteAccess {
public:
  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
  cycles access_time() const override { return cycles{4}; }

  static std::shared_ptr<ImmediateByte> instance();
};

class IndirectByte : public ByteAccess {
  std::shared_ptr<WordAccess> m_pointer;

public:
  explicit IndirectByte(std::shared_ptr<WordAccess> pointer)
      : m_pointer(std::move(pointer)) {}

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
  cycles access_time() const override {
    return cycles{4} + m_pointer->access_time();
  }

  static std::shared_ptr<IndirectByte>
  from(std::shared_ptr<WordAccess> pointer);
};

class IncrementingWord : public WordAccess {
  std::shared_ptr<WordAccess> m_inner;

public:
  explicit IncrementingWord(std::shared_ptr<WordAccess> inner)
      : m_inner(std::move(inner)) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;

  static std::shared_ptr<IncrementingWord>
  from(std::shared_ptr<WordAccess> inner);
};

class DecrementingWord : public WordAccess {
  std::shared_ptr<WordAccess> m_inner;

public:
  explicit DecrementingWord(std::shared_ptr<WordAccess> inner)
      : m_inner(std::move(inner)) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;

  static std::shared_ptr<DecrementingWord>
  from(std::shared_ptr<WordAccess> inner);
};

class PreDecrementingWord : public WordAccess {
  std::shared_ptr<WordAccess> m_inner;

public:
  explicit PreDecrementingWord(std::shared_ptr<WordAccess> inner)
      : m_inner(std::move(inner)) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;

  static std::shared_ptr<PreDecrementingWord>
  from(std::shared_ptr<WordAccess> inner);
};

class WordRegister : public WordAccess {
  CPU::R16 m_reg;

public:
  explicit WordRegister(CPU::R16 reg) : m_reg(reg) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;

  static std::shared_ptr<WordRegister> bc();
  static std::shared_ptr<WordRegister> de();
  static std::shared_ptr<WordRegister> hl();
  static std::shared_ptr<WordRegister> sp();
};

class DoubleByteWord : public WordAccess {
  std::shared_ptr<ByteAccess> m_high;
  std::shared_ptr<ByteAccess> m_low;

public:
  DoubleByteWord(std::shared_ptr<ByteAccess> high,
                 std::shared_ptr<ByteAccess> low)
      : m_high(std::move(high)), m_low(std::move(low)) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override {
    return m_high->access_time() + m_low->access_time();
  }

  static std::shared_ptr<DoubleByteWord> from(std::shared_ptr<ByteAccess> high,
                                              std::shared_ptr<ByteAccess> low);
};

class DelayedWordAccess : public WordAccess {
  std::shared_ptr<WordAccess> m_inner;

public:
  explicit DelayedWordAccess(std::shared_ptr<WordAccess> inner)
      : m_inner(std::move(inner)) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override {
    return m_inner->read(registers, memory);
  }
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override {
    m_inner->write(registers, memory, value);
  }
  cycles access_time() const override {
    return cycles{4} + m_inner->access_time();
  }

  static std::shared_ptr<DelayedWordAccess>
  from(std::shared_ptr<WordAccess> inner) {
    return std::make_shared<DelayedWordAccess>(inner);
  }
};

class OffsatWord : public WordAccess {
  std::shared_ptr<WordAccess> m_access;
  std::shared_ptr<ByteAccess> m_offset;

public:
  OffsatWord(std::shared_ptr<WordAccess> access,
             std::shared_ptr<ByteAccess> offset)
      : m_access(std::move(access)), m_offset(std::move(offset)) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override {
    return m_offset->access_time() + m_access->access_time();
  }

  static std::shared_ptr<OffsatWord> from(std::shared_ptr<WordAccess> access,
                                          std::shared_ptr<ByteAccess> offset) {
    return std::make_shared<OffsatWord>(access, offset);
  }
};

class IndirectWord : public WordAccess {
  std::shared_ptr<WordAccess> m_pointer;

public:
  explicit IndirectWord(std::shared_ptr<WordAccess> pointer)
      : m_pointer(std::move(pointer)) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override {
    return cycles{8} + m_pointer->access_time();
  }

  static std::shared_ptr<IndirectWord>
  from(std::shared_ptr<WordAccess> pointer) {
    return std::make_shared<IndirectWord>(std::move(pointer));
  }
};

} // namespace data_access

namespace instructions {
class ByteLoad : public Instruction {
  std::shared_ptr<data_access::ByteAccess> m_destination;
  std::shared_ptr<const data_access::ByteAccess> m_source;

public:
  ByteLoad(std::shared_ptr<data_access::ByteAccess> dest,
           std::shared_ptr<const data_access::ByteAccess> src);

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

class WordLoad : public Instruction {
  std::shared_ptr<data_access::WordAccess> m_destination;
  std::shared_ptr<const data_access::WordAccess> m_source;

public:
  WordLoad(std::shared_ptr<data_access::WordAccess> dest,
           std::shared_ptr<const data_access::WordAccess> src);
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

} // namespace instructions
} // namespace greenboy
