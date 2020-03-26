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
                         MemoryBus &memory) const = 0;
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
 * @brief Loads content of one register into another
 *
 */
class LOAD_R8_R8 : public Instruction {
  CPU::R8 m_to;
  CPU::R8 m_from;

public:
  constexpr LOAD_R8_R8(CPU::R8 to, CPU::R8 from) : m_to(to), m_from(from) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads immediate value int to a register
 *
 */
class LOAD_R8_n : public Instruction {
  CPU::R8 m_to;

public:
  explicit constexpr LOAD_R8_n(CPU::R8 to) : m_to(to) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the memory at the address pointed to by HL
 * into a register.
 *
 */
class LOAD_R8_HL : public Instruction {
  CPU::R8 m_to;

public:
  explicit constexpr LOAD_R8_HL(CPU::R8 to) : m_to(to) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the values in a register into the memory at the address
 * specified by HL.
 *
 */
class LOAD_HL_R8 : public Instruction {
  CPU::R8 m_from;

public:
  explicit constexpr LOAD_HL_R8(CPU::R8 from) : m_from(from) {}

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the memory at the address pointed to by the
 * 16 bit register into register A.
 *
 */
class LOAD_A_R16 : public Instruction {
  CPU::R16 m_source;

public:
  explicit constexpr LOAD_A_R16(CPU::R16 source) : m_source(source) {}

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
 * @brief Loads the value store at the memory at the address pointed to by
 * register C + ff00 into register A.
 *
 */
class LOAD_A_C : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the value in register A into the memory at
 * the address pointed to by register C + ff00.
 *
 */
class LOAD_C_A : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the memory at the address pointed to by the
 * immediate value + ff00 into register A.
 *
 */
class LOAD_A_n : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the value in register A into the memory at
 * the address pointed to by the immediate value + ff00.
 *
 */
class LOAD_n_A : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the memory at the address pointed to by the
 * immediate word value into register A.
 *
 */
class LOAD_A_nn : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the value store at the value in register A into the memory at
 * the address pointed to by the immediate word value.
 *
 */
class LOAD_nn_A : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the immediate 8-bit value into the memory at the address pointed
 * to by HL.
 *
 */
class LOAD_HL_n : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

/**
 * @brief Loads the immediate 16-bit value into the register.
 *
 */
class LOAD_R16_nn : public Instruction {
  CPU::R16 m_destination;

public:
  explicit constexpr LOAD_R16_nn(CPU::R16 destination)
      : m_destination(destination) {}

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

  cycles execute(CPU::RegisterSet &registers,
                 MemoryBus & /* memory */) const override;
};

} // namespace instructions
} // namespace greenboy
