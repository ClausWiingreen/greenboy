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
  //! @cond Doxygen_Suppress
  Instruction() = default;
  Instruction(const Instruction &) = delete;
  Instruction(Instruction &&) = delete;

  virtual ~Instruction() = default;

  Instruction &operator=(const Instruction &) = delete;
  Instruction &operator=(Instruction &&) = delete;
  //! @endcond

  /**
   * @brief executes the instruction
   *
   * @param registers the non memorymapped registers which may be modified by
   * the instruction
   * @param memory the memory map which may be written to or read from
   * @return the number of cpu cycles it took to execute this instruction
   */
  virtual cycles execute(CPU::RegisterSet &registers,
                         [[maybe_unused]] MemoryBus &memory) const = 0;
};

enum class R8 { B, C, D, E, H, L };

template <R8 Reg> constexpr byte &reg(CPU::RegisterSet &registers) {
  switch (Reg) {
  case R8::B:
    return registers.b;
  case R8::C:
    return registers.c;
  case R8::D:
    return registers.d;
  case R8::E:
    return registers.e;
  case R8::H:
    return registers.h;
  case R8::L:
    return registers.l;
  }
}

enum class R16 { BC, DE, HL };

class register_pair {
  byte &m_high;
  byte &m_low;

public:
  constexpr register_pair(byte &high, byte &low) : m_high(high), m_low(low) {}

  constexpr byte &high() { return m_high; }
  constexpr byte &low() { return m_low; }
  constexpr const byte &high() const { return m_high; }
  constexpr const byte &low() const { return m_low; }
};

template <R16 Reg> constexpr register_pair reg(CPU::RegisterSet &registers) {
  switch (Reg) {
  case R16::BC:
    return register_pair(registers.b, registers.c);
  case R16::DE:
    return register_pair(registers.d, registers.e);
  case R16::HL:
    return register_pair(registers.h, registers.l);
  }
}

namespace instructions {
/**
 * @brief a no operation instruction.
 * advanced the pc by one over 4 cycles.
 */
class NOP : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override;
};

/**
 * @brief pushes the address after this instruction to the stack and loads the
 * immediate 16-bit value into the PC.
 *
 */
class CALL : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override;
};

/**
 * @brief Pops a 16-bit value from the stack into the PC.
 *
 */
class RET : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override;
};

/**
 * @brief pushes the current value of the PC to the memory stack and loads to
 * the PC the page 0 memory address. The next instruction is fetched from the
 * address specified by the new content of PC.
 *
 * @tparam Address the address to jump to
 */
template <int Address> class RST : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;

    memory.write(registers.sp--, registers.pc.high());
    memory.write(registers.sp--, registers.pc.low());

    registers.pc = word(Address);
    return cycles{16};
  }
};

/**
 * @brief Loads content of one register into another
 *
 * @tparam To the register to read from
 * @tparam From the register to write to
 */
template <R8 To, R8 From> class LOAD_R8_R8 : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    reg<To>(registers) = reg<From>(registers);
    return cycles{4};
  }
};

/**
 * @brief Loads immediate value int to a register
 *
 * @tparam To the register to write to
 */
template <R8 To> class LOAD_R8_n : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    reg<To>(registers) = memory.read(registers.pc++);
    return cycles{8};
  }
};

/**
 * @brief Loads the value store at the memory at the address pointed to by HL
 * into a register.
 *
 * @tparam To the register to write the value to.
 */
template <R8 To> class LOAD_R8_HL : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    reg<To>(registers) = memory.read(word(registers.l, registers.h));
    return cycles{8};
  }
};

/**
 * @brief Loads the values in a register into the memory at the address
 * specified by HL.
 *
 * @tparam From the register to read from.
 */
template <R8 From> class LOAD_HL_R8 : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    memory.write(word(registers.l, registers.h), reg<From>(registers));
    return cycles{8};
  }
};

/**
 * @brief Loads the value store at the memory at the address pointed to by the
 * 16 bit register into register A.
 *
 * @tparam From the register to read from.
 */
template <R16 From> class LOAD_A_R16 : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    auto word_register = reg<From>(registers);
    registers.a = memory.read(word{word_register.low(), word_register.high()});
    return cycles{8};
  }
};

/**
 * @brief Loads the value store at the memory at the address pointed to by HL
 * into register A and increments HL.
 */
class LOAD_A_HLI : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    registers.a = memory.read(word{registers.l, registers.h});
    registers.l++;
    if (registers.l == byte{0}) {
      registers.h++;
    }
    return cycles{8};
  }
};

/**
 * @brief Loads the value store at the memory at the address pointed to by HL
 * into register A and increments HL.
 */
class LOAD_A_HLD : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    registers.a = memory.read(word{registers.l, registers.h});
    registers.l--;
    if (registers.l == byte{0xff}) {
      registers.h--;
    }
    return cycles{8};
  }
};

/**
 * @brief Loads the value store at the memory at the address pointed to by HL
 * into register A and increments HL.
 */
class LOAD_HLI_A : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    memory.write(word{registers.l, registers.h}, registers.a);
    registers.l++;
    if (registers.l == byte{0}) {
      registers.h++;
    }
    return cycles{8};
  }
};

/**
 * @brief Loads the value store at the memory at the address pointed to by HL
 * into register A and increments HL.
 */
class LOAD_HLD_A : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    memory.write(word{registers.l, registers.h}, registers.a);
    registers.l--;
    if (registers.l == byte{0xff}) {
      registers.h--;
    }
    return cycles{8};
  }
};

/**
 * @brief Loads the value store at the memory at the address pointed to by
 * register C + ff00 into register A.
 *
 */
class LOAD_A_C : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    registers.a = memory.read(word{registers.c, byte{0xff}});
    return cycles{8};
  }
};

/**
 * @brief Loads the value store at the value in register A into the memory at
 * the address pointed to by register C + ff00.
 *
 */
class LOAD_C_A : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    memory.write(word{registers.c, byte{0xff}}, registers.a);
    return cycles{8};
  }
};

/**
 * @brief Loads the value store at the memory at the address pointed to by the
 * immediate value + ff00 into register A.
 *
 */
class LOAD_A_n : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    registers.a = memory.read(word{memory.read(registers.pc++), byte{0xff}});
    return cycles{12};
  }
};

/**
 * @brief Loads the value store at the value in register A into the memory at
 * the address pointed to by the immediate value + ff00.
 *
 */
class LOAD_n_A : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    memory.write(word{memory.read(registers.pc++), byte{0xff}}, registers.a);
    return cycles{12};
  }
};

/**
 * @brief Loads the value store at the memory at the address pointed to by the
 * immediate word value into register A.
 *
 */
class LOAD_A_nn : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    auto low = memory.read(registers.pc++);
    auto high = memory.read(registers.pc++);
    registers.a = memory.read(word{low, high});
    return cycles{16};
  }
};

/**
 * @brief Loads the value store at the value in register A into the memory at
 * the address pointed to by the immediate word value.
 *
 */
class LOAD_nn_A : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    auto low = memory.read(registers.pc++);
    auto high = memory.read(registers.pc++);
    memory.write(word{low, high}, registers.a);
    return cycles{16};
  }
};

/**
 * @brief Loads the immediate 8-bit value into the memory at the address pointed
 * to by HL.
 *
 */
class LOAD_HL_n : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override;
};

/**
 * @brief Loads the immediate 16-bit value into the register.
 *
 * @tparam To the register to write to.
 */
template <R16 To> class LOAD_R16_nn : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    ++registers.pc;
    auto [high, low] = reg<To>(registers);
    low.get() = memory.read(registers.pc++);
    high.get() = memory.read(registers.pc++);
    return cycles{12};
  }
};

/**
 * @brief Instruction that writes 1 to a given bit on a given register
 *
 * @tparam Bit the bit that is set should be between 0 and 7
 * @tparam Reg the 8 bit register where the bit is sat
 */
template <unsigned Bit, R8 Reg> class SET : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    registers.pc++;
    registers.pc++;

    reg<Reg>(registers) |= byte{1u << Bit};
    return cycles{8};
  }
};

/**
 * @brief Instructions that writes 0 to a given bit on a given register.
 *
 * @tparam Bit the bit that is reset, should be between 0 and 7
 * @tparam Reg the 8 bit register where the bit is reset
 */
template <unsigned Bit, R8 Reg> class RES : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    registers.pc++;
    registers.pc++;

    reg<Reg>(registers) &= byte{static_cast<uint8_t>(~(1u << Bit))};
    return cycles{8};
  }
};

} // namespace instructions
} // namespace greenboy
