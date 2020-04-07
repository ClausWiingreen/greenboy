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
/**
 * @brief a no operation instruction.
 * advanced the pc by one over 4 cycles.
 */
class NoOperation : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief pushes the address after this instruction to the stack and loads the
 * immediate 16-bit value into the PC.
 *
 */
class Call : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Pops a 16-bit value from the stack into the PC.
 *
 */
class RET : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief pushes the current value of the PC to the memory stack and loads to
 * the PC the page 0 memory address. The next instruction is fetched from the
 * address specified by the new content of PC.
 *
 */
class RST : public Instruction {
  word m_address;

public:
  explicit RST(word address) : m_address{address} {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the memory at the address pointed to by HL
 * into register A and increments HL.
 */
class LOAD_A_HLI : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the memory at the address pointed to by HL
 * into register A and increments HL.
 */
class LOAD_A_HLD : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the memory at the address pointed to by HL
 * into register A and increments HL.
 */
class LOAD_HLI_A : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the memory at the address pointed to by HL
 * into register A and increments HL.
 */
class LOAD_HLD_A : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Instructions that moves the value of SP + e to register HL
 *
 */
class LOAD_HL_SP_e : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

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

class ByteConstantAccess : public ByteAccess {
  byte m_value;

public:
  explicit ByteConstantAccess(byte value) : m_value(value) {}

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

  static std::shared_ptr<ByteRegisterAccess> b();
  static std::shared_ptr<ByteRegisterAccess> c();
  static std::shared_ptr<ByteRegisterAccess> d();
  static std::shared_ptr<ByteRegisterAccess> e();
  static std::shared_ptr<ByteRegisterAccess> h();
  static std::shared_ptr<ByteRegisterAccess> l();
  static std::shared_ptr<ByteRegisterAccess> a();
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
  IndirectAndIncrementByteAccess(std::shared_ptr<WordAccess> pointer)
      : m_inner(pointer), m_pointer(pointer) {}

  byte read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class IndirectAndDecrementByteAccess : public ByteAccess {
  IndirectByteAccess m_inner;
  std::shared_ptr<WordAccess> m_pointer;

public:
  IndirectAndDecrementByteAccess(std::shared_ptr<WordAccess> pointer)
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

  static std::shared_ptr<WordRegisterAccess> bc();
  static std::shared_ptr<WordRegisterAccess> de();
  static std::shared_ptr<WordRegisterAccess> hl();
  static std::shared_ptr<WordRegisterAccess> af();
  static std::shared_ptr<WordRegisterAccess> sp();
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

class IndirectWordAccess : public WordAccess {
  std::shared_ptr<WordAccess> m_pointer;

public:
  IndirectWordAccess(std::shared_ptr<WordAccess> pointer)
      : m_pointer(std::move(pointer)) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
};

class IndirectAndIncrementWordAccess : public WordAccess {
  IndirectWordAccess m_inner;
  std::shared_ptr<WordAccess> m_pointer;

public:
  IndirectAndIncrementWordAccess(std::shared_ptr<WordAccess> pointer)
      : m_inner(pointer), m_pointer(pointer) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
};

class IndirectAndDecrementWordAccess : public WordAccess {
  IndirectWordAccess m_inner;
  std::shared_ptr<WordAccess> m_pointer;

public:
  IndirectAndDecrementWordAccess(std::shared_ptr<WordAccess> pointer)
      : m_inner(pointer), m_pointer(pointer) {}

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
};

//

class ByteLoad : public Instruction {
  std::shared_ptr<ByteAccess> m_destination;
  std::shared_ptr<const ByteAccess> m_source;

public:
  ByteLoad(std::shared_ptr<ByteAccess> dest,
           std::shared_ptr<const ByteAccess> src);

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

class WordLoad : public Instruction {
  std::shared_ptr<WordAccess> m_destination;
  std::shared_ptr<const WordAccess> m_source;

public:
  WordLoad(std::shared_ptr<WordAccess> dest,
           std::shared_ptr<const WordAccess> src)
      : m_destination(std::move(dest)), m_source(std::move(src)) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Instructions that pushes the value of a 16 bit register to the stack
 *
 */
class Push : public Instruction {
  std::shared_ptr<WordRegisterAccess> m_register;

public:
  explicit Push(std::shared_ptr<WordRegisterAccess> reg)
      : m_register(std::move(reg)) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Instructions that pops a 16 bit value from the stack and places it
 * into the 16 bit register
 *
 */
class Pop : public Instruction {
  std::shared_ptr<WordRegisterAccess> m_register;

public:
  explicit Pop(std::shared_ptr<WordRegisterAccess> reg)
      : m_register(std::move(reg)) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Instruction that writes 1 to a given bit on a given register
 *
 */
class SetBit : public Instruction {
  unsigned m_bit;
  std::shared_ptr<ByteRegisterAccess> m_reg;

public:
  SetBit(unsigned bit, std::shared_ptr<ByteRegisterAccess> reg)
      : m_bit(bit), m_reg(std::move(reg)) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Instructions that writes 0 to a given bit on a given register.
 *
 */
class ResetBit : public Instruction {
  unsigned m_bit;
  std::shared_ptr<ByteRegisterAccess> m_reg;

public:
  ResetBit(unsigned bit, std::shared_ptr<ByteRegisterAccess> reg)
      : m_bit(bit), m_reg(std::move(reg)) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

// only here for checking and consistency
inline void examples() {
  ByteLoad ld_r_r{ByteRegisterAccess::b(), ByteRegisterAccess::c()};
  ByteLoad ld_r_n{ByteRegisterAccess::d(),
                  std::make_shared<ImmediateByteAccess>()};
  ByteLoad le_r_HL{
      ByteRegisterAccess::e(),
      std::make_shared<IndirectByteAccess>(WordRegisterAccess::hl())};
  ByteLoad ld_HL_r{
      std::make_shared<IndirectByteAccess>(WordRegisterAccess::hl()),
      ByteRegisterAccess::h()};
  ByteLoad ld_A_DE{
      ByteRegisterAccess::a(),
      std::make_shared<IndirectByteAccess>(WordRegisterAccess::de())};
  ByteLoad ld_A_C{ByteRegisterAccess::a(),
                  std::make_shared<IndirectByteAccess>(
                      std::make_shared<DoubleByteWordAccess>(
                          std::make_shared<ByteConstantAccess>(byte{0xff}),
                          ByteRegisterAccess::c()))};
  ByteLoad ld_C_A{std::make_shared<IndirectByteAccess>(
                      std::make_shared<DoubleByteWordAccess>(
                          std::make_shared<ByteConstantAccess>(byte{0xff}),
                          ByteRegisterAccess::c())),
                  ByteRegisterAccess::a()};
  ByteLoad ld_A_n{ByteRegisterAccess::a(),
                  std::make_shared<IndirectByteAccess>(
                      std::make_shared<DoubleByteWordAccess>(
                          std::make_shared<ByteConstantAccess>(byte{0xff}),
                          std::make_shared<ImmediateByteAccess>()))};
  ByteLoad ld_n_A{std::make_shared<IndirectByteAccess>(
                      std::make_shared<DoubleByteWordAccess>(
                          std::make_shared<ByteConstantAccess>(byte{0xff}),
                          std::make_shared<ImmediateByteAccess>())),
                  ByteRegisterAccess::a()};
  ByteLoad ld_nn_A{std::make_shared<IndirectByteAccess>(
                       std::make_shared<ImmediateWordAccess>()),
                   ByteRegisterAccess::a()};
  ByteLoad ld_A_nn{ByteRegisterAccess::a(),
                   std::make_shared<IndirectByteAccess>(
                       std::make_shared<ImmediateWordAccess>())};
  ByteLoad ld_A_HLI{ByteRegisterAccess::a(),
                    std::make_shared<IndirectAndIncrementByteAccess>(
                        WordRegisterAccess::hl())};
  ByteLoad ld_A_HLD{ByteRegisterAccess::a(),
                    std::make_shared<IndirectAndDecrementByteAccess>(
                        WordRegisterAccess::hl())};
  ByteLoad ld_HLI_A{std::make_shared<IndirectAndIncrementByteAccess>(
                        WordRegisterAccess::hl()),
                    ByteRegisterAccess::a()};
  ByteLoad ld_HLD_A{std::make_shared<IndirectAndDecrementByteAccess>(
                        WordRegisterAccess::hl()),
                    ByteRegisterAccess::a()};
}
} // namespace instructions
} // namespace greenboy
