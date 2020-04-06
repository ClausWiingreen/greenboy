#pragma once
#include <functional>
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
class NOP : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief pushes the address after this instruction to the stack and loads the
 * immediate 16-bit value into the PC.
 *
 */
class CALL : public Instruction {
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
 * @brief Instruction that writes 1 to a given bit on a given register
 *
 */
class SET : public Instruction {
  unsigned m_bit;
  CPU::R8 m_reg;

public:
  constexpr SET(unsigned bit, CPU::R8 reg) : m_bit(bit), m_reg(reg) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Instructions that writes 0 to a given bit on a given register.
 *
 */
class RES : public Instruction {
  unsigned m_bit;
  CPU::R8 m_reg;

public:
  constexpr RES(unsigned bit, CPU::R8 reg) : m_bit(bit), m_reg(reg) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Instructions that pushes the value of a 16 bit register to the stack
 *
 */
class PUSH_R16 : public Instruction {
  CPU::R16 m_register;

public:
  explicit constexpr PUSH_R16(CPU::R16 reg) : m_register(reg) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Instructions that pops a 16 bit value from the stack and places it
 * into the 16 bit register
 *
 */
class POP_R16 : public Instruction {
  CPU::R16 m_register;

public:
  explicit constexpr POP_R16(CPU::R16 reg) : m_register(reg) {}

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

/**
 * @brief Instructions that stored the value of SP to the memory pointed to by
 * the immediated address
 *
 */
class LOAD_nn_SP : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

class ByteAccess {
public:
  virtual ~ByteAccess() = default;

  virtual byte read(CPU::RegisterSet &registers,
                    const MemoryBus &memory) const = 0;
  virtual void write(CPU::RegisterSet &registers, MemoryBus &memory,
                     byte value) = 0;
};

class ByteConstantAccess : public ByteAccess {
  byte m_value;

public:
  ByteConstantAccess(byte value) : m_value(value) {}

  byte read(CPU::RegisterSet &registers,
            const MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class ByteRegisterAccess : public ByteAccess {
  CPU::R8 m_reg;

public:
  explicit ByteRegisterAccess(CPU::R8 reg) : m_reg(reg) {}

  byte read(CPU::RegisterSet &registers,
            const MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class ImmediateByteAccess : public ByteAccess {
public:
  byte read(CPU::RegisterSet &registers,
            const MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class WordAccess {
public:
  virtual ~WordAccess() = default;

  virtual word read(CPU::RegisterSet &registers,
                    const MemoryBus &memory) const = 0;
  virtual void write(CPU::RegisterSet &registers, MemoryBus &memory,
                     word value) = 0;
};

class IndirectByteAccess : public ByteAccess {
  std::shared_ptr<WordAccess> m_pointer;

public:
  explicit IndirectByteAccess(std::shared_ptr<WordAccess> pointer)
      : m_pointer(std::move(pointer)) {}

  byte read(CPU::RegisterSet &registers,
            const MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             byte value) override;
};

class WordRegisterAccess : public WordAccess {
  CPU::R16 m_reg;

public:
  explicit WordRegisterAccess(CPU::R16 reg) : m_reg(reg) {}

  word read(CPU::RegisterSet &registers,
            const MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
};

class ImmediateWordAccess : public WordAccess {
  ImmediateByteAccess m_access;

public:
  word read(CPU::RegisterSet &registers,
            const MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
};

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
      : m_destination(dest), m_source(src) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};
} // namespace instructions
} // namespace greenboy
