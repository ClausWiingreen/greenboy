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
                         MemoryBus &memory) const = 0;
};

namespace instructions {
/**
 * @brief a no operation instruction.
 * advanced the pc by one over 4 cycles.
 */
class NOP : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 MemoryBus & /*memory*/) const override;
};

class CALL : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

class RET : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

template <int Address> class RST : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 MemoryBus &memory) const override {
    ++registers.pc;

    memory.write(registers.sp--, registers.pc.high());
    memory.write(registers.sp--, registers.pc.low());

    registers.pc = word(Address);
    return cycles{16};
  }
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

template <R8 To, R8 From> class LOAD_R8_R8 : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 MemoryBus & /*memory*/) const override {
    ++registers.pc;
    reg<To>(registers) = reg<From>(registers);
    return cycles{4};
  }
};

template <R8 To> class LOAD_R8_n : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 MemoryBus &memory) const override {
    ++registers.pc;
    reg<To>(registers) = memory.read(registers.pc++);
    return cycles{8};
  }
};

template <R8 To> class LOAD_R8_HL : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 MemoryBus &memory) const override {
    ++registers.pc;
    reg<To>(registers) = memory.read(word(registers.l, registers.h));
    return cycles{8};
  }
};

template <R8 From> class LOAD_HL_R8 : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 MemoryBus &memory) const override {
    ++registers.pc;
    memory.write(word(registers.l, registers.h), reg<From>(registers));
    return cycles{8};
  }
};

class LOAD_HL_n : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

enum class R16 { BC, DE, HL };

template <R16 Reg> constexpr auto &reg(CPU::RegisterSet &registers) {
  switch (Reg) {
  case R16::BC:
    return std::make_pair(std::ref(registers.b), std::ref(registers.c));
  case R16::DE:
    return std::make_pair(std::ref(registers.d), std::ref(registers.e));
  case R16::HL:
    return std::make_pair(std::ref(registers.h), std::ref(registers.l));
  }
}

template <R16 To> class LOAD_R16_nn : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 MemoryBus &memory) const override {
    ++registers.pc;
    auto [high, low] = reg<To>(registers);
    low.get() = memory.read(registers.pc++);
    high.get() = memory.read(registers.pc++);
    return cycles{12};
  }
};

template <int bit, R8 Reg> class SET : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    registers.pc++;
    registers.pc++;

    reg<Reg>(registers) |= 1 << bit;
    return cycles{8};
  }
};

template <int bit, R8 Reg> class RES : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 [[maybe_unused]] MemoryBus &memory) const override {
    registers.pc++;
    registers.pc++;

    reg<Reg>(registers) &= ~(1 << bit);
    return cycles{8};
  }
};

} // namespace instructions
} // namespace greenboy
