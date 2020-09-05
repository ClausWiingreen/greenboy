#pragma once

#include "../cpu.hpp"
#include "../memory_bus.hpp"
#include "../timing.hpp"
#include "../types.hpp"

#include <type_traits>

namespace greenboy {

template <typename T, typename U>
struct enable_if_same : std::enable_if_t<std::is_same_v<T, U>> {};

template <typename T, typename = void> struct is_readable : std::false_type {};

template <typename T>
struct is_readable<
    T, std::void_t<typename T::value_type,
                   enable_if_same<decltype(T::read),
                                  typename T::value_type (*)(CPU::RegisterSet &,
                                                             MemoryBus &)>>>
    : std::true_type {};

template <typename T, typename = void> struct is_writeable : std::false_type {};

template <typename T>
struct is_writeable<
    T, std::void_t<typename T::value_type,
                   enable_if_same<decltype(T::write),
                                  void (*)(typename T::value_type,
                                           CPU::RegisterSet &, MemoryBus &)>>>
    : std::true_type {};

template <typename T> constexpr bool is_readable_v = is_readable<T>::value;
template <typename T> constexpr bool is_writeable_v = is_writeable<T>::value;

namespace instructions {

namespace registers {
enum class Reg { B, C, D, E, H, L, A, BC, DE, HL, AF, SP };

template <Reg Name> struct Register {
  static auto read(CPU::RegisterSet &registers, MemoryBus & /* memory */) {
    if constexpr (Name == Reg::B)
      return registers.b;
    else if constexpr (Name == Reg::C)
      return registers.c;
    else if constexpr (Name == Reg::D)
      return registers.d;
    else if constexpr (Name == Reg::E)
      return registers.e;
    else if constexpr (Name == Reg::H)
      return registers.h;
    else if constexpr (Name == Reg::L)
      return registers.l;
    else if constexpr (Name == Reg::A)
      return registers.a;
    else if constexpr (Name == Reg::BC)
      return to_word(registers.b, registers.c);
    else if constexpr (Name == Reg::DE)
      return to_word(registers.d, registers.e);
    else if constexpr (Name == Reg::HL)
      return to_word(registers.h, registers.l);
    else if constexpr (Name == Reg::AF)
      return to_word(registers.a, static_cast<byte>(registers.f));
    else if constexpr (Name == Reg::SP)
      return registers.sp;
  }
  using value_type = decltype(
      read(std::declval<CPU::RegisterSet &>(), std::declval<MemoryBus &>()));
  static void write(value_type value, CPU::RegisterSet &registers,
                    MemoryBus & /* memory */) {
    if constexpr (Name == Reg::B)
      registers.b = value;
    else if constexpr (Name == Reg::C)
      registers.c = value;
    else if constexpr (Name == Reg::D)
      registers.d = value;
    else if constexpr (Name == Reg::E)
      registers.e = value;
    else if constexpr (Name == Reg::H)
      registers.h = value;
    else if constexpr (Name == Reg::L)
      registers.l = value;
    else if constexpr (Name == Reg::A)
      registers.a = value;
    else if constexpr (Name == Reg::BC) {
      registers.b = high_byte(value);
      registers.c = low_byte(value);
    } else if constexpr (Name == Reg::DE) {
      registers.d = high_byte(value);
      registers.e = low_byte(value);
    } else if constexpr (Name == Reg::HL) {
      registers.h = high_byte(value);
      registers.l = low_byte(value);
    } else if constexpr (Name == Reg::AF) {
      registers.a = high_byte(value);
      registers.f = low_byte(value);
    } else if constexpr (Name == Reg::SP)
      registers.sp = value;
  }
  static constexpr cycles access_time() { return cycles{0}; }
};

using B = Register<Reg::B>;
using C = Register<Reg::C>;
using D = Register<Reg::D>;
using E = Register<Reg::E>;
using H = Register<Reg::H>;
using L = Register<Reg::L>;
using A = Register<Reg::A>;

using BC = Register<Reg::BC>;
using DE = Register<Reg::DE>;
using HL = Register<Reg::HL>;
using AF = Register<Reg::AF>;
using SP = Register<Reg::SP>;

} // namespace registers

template <class ValueType = byte> struct Immediate {
  using value_type = typename ValueType;
  static byte read(CPU::RegisterSet &registers, MemoryBus &memory) {
    if constexpr (std::is_same_v<value_type, word>) {
      auto low = memory.read(registers.pc++);
      auto high = memory.read(registers.pc++);
      return to_word(high, low);
    } else {
      return memory.read(registers.pc++);
    }
  }
  static constexpr cycles access_time() {
    if constexpr (std::is_same_v<value_type, word>) {
      return cycles{8};
    } else {
      return cycles{4};
    }
  }
};

template <class Register, class ValueType = byte> struct Indirect {
  using value_type = ValueType;
  static value_type read(CPU::RegisterSet &registers, MemoryBus &memory) {
    static_assert(is_readable_v<Register>);
    static_assert(std::is_same_v<typename Register::value_type, word>);

    auto address = Register::read(registers, memory);
    if constexpr (std::is_same_v<value_type, word>) {
      auto low = memory.read(address);
      auto high = memory.read(address + 1);
      return to_word(high, low);
    } else {
      return memory.read(address);
    }
  }
  static void write(value_type value, CPU::RegisterSet &registers,
                    MemoryBus &memory) {
    static_assert(is_readable_v<Register>);
    static_assert(std::is_same_v<typename Register::value_type, word>);

    auto address = Register::read(registers, memory);
    if constexpr (std::is_same_v<value_type, word>) {
      memory.write(address, low_byte(value));
      memory.write(address + 1, high_byte(value));
    } else {
      memory.write(address, value);
    }
  }
  static constexpr cycles access_time() {
    return cycles{4} + Register::access_time();
  }
};

template <class Data, class = typename Data::value_type> struct Increment;

template <class Data> struct Increment<Data, word> {
  using value_type = word;
  static word read(CPU::RegisterSet &registers, MemoryBus &memory) {
    static_assert(is_readable_v<Data>);
    static_assert(is_writeable_v<Data>);

    auto value = Data::read(registers, memory);
    Data::write(value + 1, registers, memory);
    return value;
  }
  static constexpr cycles access_time() { return Data::access_time() * 2; }
};

template <class Data, class = typename Data::value_type> struct Decrement;

template <class Data> struct Decrement<Data, word> {
  using value_type = word;
  static word read(CPU::RegisterSet &registers, MemoryBus &memory) {
    static_assert(is_writeable_v<Data>);
    static_assert(is_readable_v<Data>);

    auto value = Data::read(registers, memory);
    Data::write(value - 1, registers, memory);
    return value;
  }
  static constexpr cycles access_time() { return Data::access_time() * 2; }
};

template <class High, class Low> struct Double {
  using value_type = word;
  static word read(CPU::RegisterSet &registers, MemoryBus &memory) {
    static_assert(is_readable_v<High>);
    static_assert(is_readable_v<Low>);
    auto high = High::read(registers, memory);
    auto low = Low::read(registers, memory);
    return to_word(high, low);
  }
  static void write(word value, CPU::RegisterSet &registers,
                    MemoryBus &memory) {
    static_assert(is_writeable_v<High>);
    static_assert(is_writeable_v<Low>);
    High::write(high_byte(value), registers, memory);
    Low::write(low_byte(value), registers, memory);
  }
  static constexpr cycles access_time() {
    return High::access_time() + Low::access_time();
  }
};

template <unsigned char Value> struct Constant {
  using value_type = byte;
  static byte read(CPU::RegisterSet &registers, MemoryBus &memory) {
    return byte{Value};
  }
  static constexpr cycles access_time() { return cycles{0}; }
};

template <class Register, class Offset> struct Offsat {
  using value_type = word;
  static word read(CPU::RegisterSet &registers, MemoryBus &memory) {
    auto value = Register::read(registers, memory);
    auto offset = to_integer<signed char>(Offset::read(registers, memory));
    auto result = value + offset;
    auto carry_bits = value ^ offset ^ result;

    registers.f.zero = false;
    registers.f.negate = false;
    registers.f.carry = (carry_bits & 0x100) != 0;
    registers.f.half_carry = (carry_bits & 0x10) != 0;

    return static_cast<word>(result);
  }
  static constexpr cycles access_time() {
    return Register::access_time() + Offset::access_time();
  }
};

template <typename Destination, typename Source>
inline cycles load(CPU::RegisterSet &registers, MemoryBus &memory) {
  static_assert(is_readable_v<Source>);
  static_assert(is_writeable_v<Destination>);
  static_assert(std::is_same_v<typename Source::value_type,
                               typename Destination::value_type>);

  auto value = Source::read(registers, memory);
  Destination::write(value, registers, memory);
  return Source::access_time() + Destination::access_time();
}

template <typename LHS, typename RHS>
inline cycles add(CPU::RegisterSet &registers, MemoryBus &memory) {
  static_assert(is_readable_v<LHS>);
  static_assert(is_readable_v<RHS>);
  static_assert(is_writeable_v<LHS>);
  static_assert(
      std::is_same_v<typename LHS::value_type, typename RHS::value_type>);

  if constexpr (std::is_same_v<typename LHS::value_type, word>) {
    auto lhs = LHS::read(registers, memory);
    auto rhs = RHS::read(registers, memory);
    auto result = lhs + rhs;
    auto carry_bits = lhs ^ rhs ^ result;

    registers.f.carry = (carry_bits & 0x10000) != 0;
    registers.f.half_carry = (carry_bits & 0x1000) != 0;
    registers.f.negate = false;
    LHS::write(static_cast<word>(result), registers, memory);
  } else {
    auto lhs = to_integer<int>(LHS::read(registers, memory));
    auto rhs = to_integer<int>(RHS::read(registers, memory));
    auto result = lhs + rhs;
    auto carry_bits = lhs ^ rhs ^ result;

    registers.f.carry = (carry_bits & 0x100) != 0;
    registers.f.zero = (result & 0xff) == 0;
    registers.f.half_carry = (carry_bits & 0x10) != 0;
    registers.f.negate = false;

    LHS::write(static_cast<byte>(result), registers, memory);
  }
  return LHS::access_time() * 2 + RHS::access_time();
}

template <typename LHS, typename RHS>
inline cycles add_with_carry(CPU::RegisterSet &registers, MemoryBus &memory) {
  static_assert(is_readable_v<LHS>);
  static_assert(is_readable_v<RHS>);
  static_assert(is_writeable_v<LHS>);
  static_assert(
      std::is_same_v<typename LHS::value_type, typename RHS::value_type>);

  auto lhs = to_integer<int>(LHS::read(registers, memory));
  auto rhs = to_integer<int>(RHS::read(registers, memory));
  auto result = lhs + rhs + (registers.f.carry ? 1 : 0);
  auto carry_bits = lhs ^ rhs ^ result;

  registers.f.carry = (carry_bits & 0x100) != 0;
  registers.f.zero = (result & 0xff) == 0;
  registers.f.half_carry = (carry_bits & 0x10) != 0;
  registers.f.negate = false;

  LHS::write(static_cast<byte>(result), registers, memory);
  return LHS::access_time() * 2 + RHS::access_time();
}

template <typename LHS, typename RHS>
inline cycles subtract(CPU::RegisterSet &registers, MemoryBus &memory) {
  static_assert(is_readable_v<LHS>);
  static_assert(is_readable_v<RHS>);
  static_assert(is_writeable_v<LHS>);
  static_assert(
      std::is_same_v<typename LHS::value_type, typename RHS::value_type>);

  auto lhs = to_integer<int>(LHS::read(registers, memory));
  auto rhs = to_integer<int>(RHS::read(registers, memory));
  auto result = lhs - rhs;
  auto carry_bits = lhs ^ rhs ^ result;

  registers.f.carry = (carry_bits & 0x100) != 0;
  registers.f.zero = (result & 0xff) == 0;
  registers.f.half_carry = (carry_bits & 0x10) != 0;
  registers.f.negate = true;

  LHS::write(static_cast<byte>(result), registers, memory);
  return LHS::access_time() * 2 + RHS::access_time();
}

template <typename LHS, typename RHS>
inline cycles subtract_with_carry(CPU::RegisterSet &registers,
                                  MemoryBus &memory) {
  static_assert(is_readable_v<LHS>);
  static_assert(is_readable_v<RHS>);
  static_assert(is_writeable_v<LHS>);
  static_assert(
      std::is_same_v<typename LHS::value_type, typename RHS::value_type>);

  auto lhs = to_integer<int>(LHS::read(registers, memory));
  auto rhs = to_integer<int>(RHS::read(registers, memory));
  auto result = lhs - rhs - (registers.f.carry ? 1 : 0);
  auto carry_bits = lhs ^ rhs ^ result;

  registers.f.carry = (carry_bits & 0x100) != 0;
  registers.f.zero = (result & 0xff) == 0;
  registers.f.half_carry = (carry_bits & 0x10) != 0;
  registers.f.negate = true;

  LHS::write(static_cast<byte>(result), registers, memory);
  return LHS::access_time() * 2 + RHS::access_time();
}

template <typename LHS, typename RHS>
inline cycles bitwise_and(CPU::RegisterSet &registers, MemoryBus &memory) {
  static_assert(is_readable_v<LHS>);
  static_assert(is_readable_v<RHS>);
  static_assert(is_writeable_v<LHS>);
  static_assert(
      std::is_same_v<typename LHS::value_type, typename RHS::value_type>);

  auto lhs = to_integer<int>(LHS::read(registers, memory));
  auto rhs = to_integer<int>(RHS::read(registers, memory));
  auto result = lhs & rhs;

  registers.f.carry = false;
  registers.f.zero = (result & 0xff) == 0;
  registers.f.half_carry = true;
  registers.f.negate = false;

  LHS::write(static_cast<byte>(result), registers, memory);
  return LHS::access_time() * 2 + RHS::access_time();
}

template <typename LHS, typename RHS>
inline cycles bitwise_or(CPU::RegisterSet &registers, MemoryBus &memory) {
  static_assert(is_readable_v<LHS>);
  static_assert(is_readable_v<RHS>);
  static_assert(is_writeable_v<LHS>);
  static_assert(
      std::is_same_v<typename LHS::value_type, typename RHS::value_type>);

  auto lhs = to_integer<int>(LHS::read(registers, memory));
  auto rhs = to_integer<int>(RHS::read(registers, memory));
  auto result = lhs | rhs;

  registers.f.carry = false;
  registers.f.zero = (result & 0xff) == 0;
  registers.f.half_carry = false;
  registers.f.negate = false;

  LHS::write(static_cast<byte>(result), registers, memory);
  return LHS::access_time() * 2 + RHS::access_time();
}

template <typename LHS, typename RHS>
inline cycles bitwise_xor(CPU::RegisterSet &registers, MemoryBus &memory) {
  static_assert(is_readable_v<LHS>);
  static_assert(is_readable_v<RHS>);
  static_assert(is_writeable_v<LHS>);
  static_assert(
      std::is_same_v<typename LHS::value_type, typename RHS::value_type>);

  auto lhs = to_integer<int>(LHS::read(registers, memory));
  auto rhs = to_integer<int>(RHS::read(registers, memory));
  auto result = lhs ^ rhs;

  registers.f.carry = false;
  registers.f.zero = (result & 0xff) == 0;
  registers.f.half_carry = false;
  registers.f.negate = false;

  LHS::write(static_cast<byte>(result), registers, memory);
  return LHS::access_time() * 2 + RHS::access_time();
}

template <typename LHS, typename RHS>
inline cycles compare(CPU::RegisterSet &registers, MemoryBus &memory) {
  static_assert(is_readable_v<LHS>);
  static_assert(is_readable_v<RHS>);
  static_assert(is_writeable_v<LHS>);
  static_assert(
      std::is_same_v<typename LHS::value_type, typename RHS::value_type>);

  auto lhs = to_integer<int>(LHS::read(registers, memory));
  auto rhs = to_integer<int>(RHS::read(registers, memory));
  auto result = lhs - rhs;
  auto carry_bits = lhs ^ rhs ^ result;

  registers.f.carry = (carry_bits & 0x100) != 0;
  registers.f.zero = (result & 0xff) == 0;
  registers.f.half_carry = (carry_bits & 0x10) != 0;
  registers.f.negate = true;
  return LHS::access_time() * 2 + RHS::access_time();
}

template <typename Register>
cycles increment(CPU::RegisterSet &registers, MemoryBus &memory) noexcept {
  if constexpr (std::is_same_v<typename Register::value_type, byte>) {
    auto carry = registers.f.carry;
    auto result = add<Register, Constant<1>>(registers, memory);
    registers.f.carry = carry;
    return result;
  } else {
    Register::write(Register::read(registers, memory) + 1, registers, memory);
    return Register::access_time() * 2;
  }
}

template <typename Register>
cycles decrement(CPU::RegisterSet &registers, MemoryBus &memory) noexcept {
  if constexpr (std::is_same_v<typename Register::value_type, byte>) {
    auto carry = registers.f.carry;
    auto result = subtract<Register, Constant<1>>(registers, memory);
    registers.f.carry = carry;
    return result;
  } else {
    Register::write(Register::read(registers, memory) - 1, registers, memory);
    return Register::access_time() * 2;
  }
}

template <typename Register>
cycles push(CPU::RegisterSet &registers, MemoryBus &memory) noexcept {
  static_assert(is_readable_v<Register>);
  static_assert(std::is_same_v<typename Register::value_type, word>);

  auto value = Register::read(registers, memory);
  auto low = low_byte(value);
  auto high = high_byte(value);
  memory.write(--registers.sp, high);
  memory.write(--registers.sp, low);
  return Register::access_time() + cycles{8};
}

template <typename Register>
cycles pop(CPU::RegisterSet &registers, MemoryBus &memory) noexcept {
  static_assert(is_writeable_v<Register>);
  static_assert(std::is_same_v<typename Register::value_type, word>);

  auto low = memory.read(registers.sp++);
  auto high = memory.read(registers.sp++);

  Register::write(to_word(high, low), registers, memory);
  return Register::access_time() + cycles{8};
}
} // namespace instructions
} // namespace greenboy
