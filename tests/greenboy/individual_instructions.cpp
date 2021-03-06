#include "greenboy/instruction.hpp"
#include "mocks/byte_access.hpp"
#include "mocks/memory_bus.hpp"
#include "mocks/word_access.hpp"
#include "gtest/gtest.h"

#include "greenboy/data_access/byte_access.hpp"
#include "greenboy/data_access/byte_register.hpp"
#include "greenboy/data_access/constant_byte.hpp"
#include "greenboy/data_access/decrementing_word.hpp"
#include "greenboy/data_access/delayed_word_access.hpp"
#include "greenboy/data_access/double_byte_word.hpp"
#include "greenboy/data_access/immediate_byte.hpp"
#include "greenboy/data_access/incrementing_word.hpp"
#include "greenboy/data_access/indirect_byte.hpp"
#include "greenboy/data_access/indirect_word.hpp"
#include "greenboy/data_access/offsat_word.hpp"
#include "greenboy/data_access/pre_decrementing_word.hpp"
#include "greenboy/data_access/word_access.hpp"
#include "greenboy/data_access/word_register.hpp"
#include "greenboy/instructions/byte_arithmetic_operation.hpp"
#include "greenboy/instructions/byte_load.hpp"
#include "greenboy/instructions/word_load.hpp"

namespace {
using namespace greenboy;
using namespace instructions;
using namespace data_access;
using ::testing::_;
using ::testing::Return;

TEST(ByteLoad, LD_A_B) {
  ByteLoad instruction{ByteRegister::a(), ByteRegister::b()};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.a = byte{0x12};
  registers.b = byte{0x34};

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0x34};
  expected_register_state.b = byte{0x34};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{0});
}

TEST(ByteLoad, LD_B_D) {
  ByteLoad instruction{ByteRegister::b(), ByteRegister::d()};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.b = byte{0x56};
  registers.d = byte{0x78};

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.b = byte{0x78};
  expected_register_state.d = byte{0x78};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{0});
}

TEST(ByteLoad, LD_B_0x24) {
  ByteLoad instruction{ByteRegister::b(), ImmediateByte::instance()};
  CPU::RegisterSet registers{};
  registers.b = byte{0x12};
  registers.pc = word{0x3432};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x3432})).WillOnce(Return(byte{0x24}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.b = byte{0x24};
  expected_register_state.pc = word{0x3433};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_H_HL) {
  ByteLoad instruction{ByteRegister::h(),
                       IndirectByte::from(WordRegister::hl())};
  CPU::RegisterSet registers{};
  registers.h = byte{0x7b};
  registers.l = byte{0xca};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x7bca})).WillOnce(Return(byte{0x5c}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.h = byte{0x5c};
  expected_register_state.l = byte{0xca};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_HL_A) {
  ByteLoad instruction{IndirectByte::from(WordRegister::hl()),
                       ByteRegister::a()};
  CPU::RegisterSet registers{};
  registers.h = byte{0x8a};
  registers.l = byte{0xc5};
  registers.a = byte{0x3c};
  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0x8ac5}, byte{0x3c}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.h = byte{0x8a};
  expected_register_state.l = byte{0xc5};
  expected_register_state.a = byte{0x3c};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_HL_n) {
  ByteLoad instruction{IndirectByte::from(WordRegister::hl()),
                       ImmediateByte::instance()};
  CPU::RegisterSet registers{};
  registers.pc = word{0x1234};
  registers.h = byte{0x8a};
  registers.l = byte{0xc5};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x1234})).WillOnce(Return(byte{0x00}));
  EXPECT_CALL(memory, write(word{0x8ac5}, byte{0x00}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x1235};
  expected_register_state.h = byte{0x8a};
  expected_register_state.l = byte{0xc5};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{8});
}

TEST(ByteLoad, LD_A_BC) {
  ByteLoad instruction{ByteRegister::a(),
                       IndirectByte::from(WordRegister::bc())};
  CPU::RegisterSet registers{};
  registers.a = byte{0x43};
  registers.b = byte{0x7b};
  registers.c = byte{0xca};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x7bca})).WillOnce(Return(byte{0x2f}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0x2f};
  expected_register_state.b = byte{0x7b};
  expected_register_state.c = byte{0xca};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_A_DE) {
  ByteLoad instruction{ByteRegister::a(),
                       IndirectByte::from(WordRegister::de())};
  CPU::RegisterSet registers{};
  registers.a = byte{0x43};
  registers.d = byte{0x7b};
  registers.e = byte{0xca};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x7bca})).WillOnce(Return(byte{0x5f}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0x5f};
  expected_register_state.d = byte{0x7b};
  expected_register_state.e = byte{0xca};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_A_ffC) {
  ByteLoad instruction{ByteRegister::a(),
                       IndirectByte::from(DoubleByteWord::from(
                           ConstantByte::from(byte{0xff}), ByteRegister::c()))};
  CPU::RegisterSet registers{};
  registers.a = byte{0x43};
  registers.c = byte{0x95};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0xff95})).WillOnce(Return(byte{0xf8}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0xf8};
  expected_register_state.c = byte{0x95};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_ffC_A) {
  ByteLoad instruction{IndirectByte::from(DoubleByteWord::from(
                           ConstantByte::from(byte{0xff}), ByteRegister::c())),
                       ByteRegister::a()};
  CPU::RegisterSet registers{};
  registers.a = byte{0x43};
  registers.c = byte{0x9f};
  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0xff9f}, byte{0x43}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0x43};
  expected_register_state.c = byte{0x9f};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_A_ff34) {
  ByteLoad instruction{
      ByteRegister::a(),
      IndirectByte::from(DoubleByteWord::from(ConstantByte::from(byte{0xff}),
                                              ImmediateByte::instance()))};
  CPU::RegisterSet registers{};
  registers.a = byte{0x43};
  registers.pc = word{0x4a22};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x4a22})).WillOnce(Return(byte{0x34}));
  EXPECT_CALL(memory, read(word{0xff34})).WillOnce(Return(byte{0xf8}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0xf8};
  expected_register_state.pc = word{0x4a23};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{8});
}

TEST(ByteLoad, LD_ff34_A) {
  ByteLoad instruction{
      IndirectByte::from(DoubleByteWord::from(ConstantByte::from(byte{0xff}),
                                              ImmediateByte::instance())),
      ByteRegister::a()};
  CPU::RegisterSet registers{};
  registers.a = byte{0x43};
  registers.pc = word{0x4a22};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x4a22})).WillOnce(Return(byte{0x34}));
  EXPECT_CALL(memory, write(word{0xff34}, byte{0x43}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0x43};
  expected_register_state.pc = word{0x4a23};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{8});
}

TEST(ByteLoad, LD_A_LY) {
  ByteLoad instruction{
      ByteRegister::a(),
      IndirectByte::from(DoubleByteWord::from(ImmediateByte::instance(),
                                              ImmediateByte::instance()))};
  CPU::RegisterSet registers{};
  registers.a = byte{0x43};
  registers.pc = word{0x4a22};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x4a22})).WillOnce(Return(byte{0x40}));
  EXPECT_CALL(memory, read(word{0x4a23})).WillOnce(Return(byte{0xff}));
  EXPECT_CALL(memory, read(word{0xff40})).WillOnce(Return(byte{0x95}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0x95};
  expected_register_state.pc = word{0x4a24};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{12});
}

TEST(ByteLoad, LD_A_0x8000) {
  ByteLoad instruction{
      ByteRegister::a(),
      IndirectByte::from(DoubleByteWord::from(ImmediateByte::instance(),
                                              ImmediateByte::instance()))};
  CPU::RegisterSet registers{};
  registers.a = byte{0x43};
  registers.pc = word{0x4a22};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x4a22})).WillOnce(Return(byte{0x00}));
  EXPECT_CALL(memory, read(word{0x4a23})).WillOnce(Return(byte{0x80}));
  EXPECT_CALL(memory, read(word{0x8000})).WillOnce(Return(byte{0x9b}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0x9b};
  expected_register_state.pc = word{0x4a24};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{12});
}

TEST(ByteLoad, LD_LY_A) {
  ByteLoad instruction{
      IndirectByte::from(DoubleByteWord::from(ImmediateByte::instance(),
                                              ImmediateByte::instance())),
      ByteRegister::a()};
  CPU::RegisterSet registers{};
  registers.a = byte{0x43};
  registers.pc = word{0x4a22};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x4a22})).WillOnce(Return(byte{0x40}));
  EXPECT_CALL(memory, read(word{0x4a23})).WillOnce(Return(byte{0xff}));
  EXPECT_CALL(memory, write(word{0xff40}, byte{0x43}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0x43};
  expected_register_state.pc = word{0x4a24};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{12});
}

TEST(ByteLoad, LD_0x8000_A) {
  ByteLoad instruction{
      IndirectByte::from(DoubleByteWord::from(ImmediateByte::instance(),
                                              ImmediateByte::instance())),
      ByteRegister::a()};
  CPU::RegisterSet registers{};
  registers.a = byte{0x43};
  registers.pc = word{0x4a22};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x4a22})).WillOnce(Return(byte{0x00}));
  EXPECT_CALL(memory, read(word{0x4a23})).WillOnce(Return(byte{0x80}));
  EXPECT_CALL(memory, write(word{0x8000}, byte{0x43}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.a = byte{0x43};
  expected_register_state.pc = word{0x4a24};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{12});
}

TEST(ByteLoad, LD_A_HLI) {
  ByteLoad instruction{
      ByteRegister::a(),
      IndirectByte::from(IncrementingWord::from(WordRegister::hl()))};
  CPU::RegisterSet registers{};
  registers.h = byte{0x01};
  registers.l = byte{0xff};
  registers.a = byte{0x22};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x01ff})).WillOnce(Return(byte{0x56}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.h = byte{0x02};
  expected_register_state.l = byte{0x00};
  expected_register_state.a = byte{0x56};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_A_HLD) {
  ByteLoad instruction{
      ByteRegister::a(),
      IndirectByte::from(DecrementingWord::from(WordRegister::hl()))};
  CPU::RegisterSet registers{};
  registers.h = byte{0x8a};
  registers.l = byte{0x5c};
  registers.a = byte{0x22};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x8a5c})).WillOnce(Return(byte{0x3c}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.h = byte{0x8a};
  expected_register_state.l = byte{0x5b};
  expected_register_state.a = byte{0x3c};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_BC_A) {
  ByteLoad instruction{IndirectByte::from(WordRegister::bc()),
                       ByteRegister::a()};
  CPU::RegisterSet registers{};
  registers.b = byte{0x20};
  registers.c = byte{0x5f};
  registers.a = byte{0x3f};

  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0x205f}, byte{0x3f}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.b = byte{0x20};
  expected_register_state.c = byte{0x5f};
  expected_register_state.a = byte{0x3f};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_DE_A) {
  ByteLoad instruction{IndirectByte::from(WordRegister::de()),
                       ByteRegister::a()};
  CPU::RegisterSet registers{};
  registers.d = byte{0x20};
  registers.e = byte{0x5c};
  registers.a = byte{0x00};

  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0x205c}, byte{0x00}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.d = byte{0x20};
  expected_register_state.e = byte{0x5c};
  expected_register_state.a = byte{0x00};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_HLI_A) {
  ByteLoad instruction{
      IndirectByte::from(IncrementingWord::from(WordRegister::hl())),
      ByteRegister::a()};
  CPU::RegisterSet registers{};
  registers.h = byte{0xff};
  registers.l = byte{0xff};
  registers.a = byte{0x56};

  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0xffff}, byte{0x56}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.h = byte{0x00};
  expected_register_state.l = byte{0x00};
  expected_register_state.a = byte{0x56};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(ByteLoad, LD_HLD_A) {
  ByteLoad instruction{
      IndirectByte::from(DecrementingWord::from(WordRegister::hl())),
      ByteRegister::a()};
  CPU::RegisterSet registers{};
  registers.h = byte{0x40};
  registers.l = byte{0x00};
  registers.a = byte{0x05};

  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0x4000}, byte{0x05}));

  auto time_passed = instruction.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.h = byte{0x3f};
  expected_register_state.l = byte{0xff};
  expected_register_state.a = byte{0x05};
  EXPECT_EQ(expected_register_state, registers);
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(WordLoad, LD_HL_0x3a5b) {
  WordLoad instruction{WordRegister::hl(),
                       DoubleByteWord::from(ImmediateByte::instance(),
                                            ImmediateByte::instance())};
  CPU::RegisterSet registers{};
  registers.pc = word{0x6bea};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x6bea})).WillOnce(Return(byte{0x5b}));
  EXPECT_CALL(memory, read(word{0x6beb})).WillOnce(Return(byte{0x3a}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(registers.pc, word{0x6bec});
  EXPECT_EQ(registers.h, byte{0x3a});
  EXPECT_EQ(registers.l, byte{0x5b});
  EXPECT_EQ(time_passed, cycles{8});
}

TEST(WordLoad, LD_SP_HL) {
  WordLoad instruction{DelayedWordAccess::from(WordRegister::sp()),
                       WordRegister::hl()};
  CPU::RegisterSet registers{};
  registers.h = byte{0x0a};
  registers.l = byte{0xbc};
  registers.sp = word{0x93f1};
  MockMemoryBus memory;

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(registers.h, byte{0x0a});
  EXPECT_EQ(registers.l, byte{0xbc});
  EXPECT_EQ(registers.sp, word{0x0abc});
  EXPECT_EQ(time_passed, cycles{4});
}

TEST(WordLoad, PUSH_BC) {
  WordLoad instruction{
      DelayedWordAccess::from(DoubleByteWord::from(
          IndirectByte::from(PreDecrementingWord::from(WordRegister::sp())),
          IndirectByte::from(PreDecrementingWord::from(WordRegister::sp())))),
      WordRegister::bc()};

  CPU::RegisterSet registers{};
  registers.b = byte{0x0a};
  registers.c = byte{0xbc};
  registers.sp = word{0xfffe};
  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0xfffd}, byte{0x0a}));
  EXPECT_CALL(memory, write(word{0xfffc}, byte{0xbc}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(registers.b, byte{0x0a});
  EXPECT_EQ(registers.c, byte{0xbc});
  EXPECT_EQ(registers.sp, word{0xfffc});
  EXPECT_EQ(time_passed, cycles{12});
}

TEST(WordLoad, POP_BC) {
  WordLoad instruction{
      WordRegister::bc(),
      DoubleByteWord::from(
          IndirectByte::from(IncrementingWord::from(WordRegister::sp())),
          IndirectByte::from(IncrementingWord::from(WordRegister::sp())))};

  CPU::RegisterSet registers{};
  registers.b = byte{0x0a};
  registers.c = byte{0xbc};
  registers.sp = word{0xfffc};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0xfffd})).WillOnce(Return(byte{0x3c}));
  EXPECT_CALL(memory, read(word{0xfffc})).WillOnce(Return(byte{0x5f}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(registers.b, byte{0x3c});
  EXPECT_EQ(registers.c, byte{0x5f});
  EXPECT_EQ(registers.sp, word{0xfffe});
  EXPECT_EQ(time_passed, cycles{8});
}

TEST(WordLoad, LDHL_SP_2) {
  WordLoad instruction{
      WordRegister::hl(),
      OffsatWord::from(DelayedWordAccess::from(WordRegister::sp()),
                       ImmediateByte::instance())};

  CPU::RegisterSet registers{};
  registers.h = byte{0x0a};
  registers.l = byte{0xbc};
  registers.f = CPU::Flags{byte{0x80}};
  registers.sp = word{0xfff8};
  registers.pc = word{0x2000};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x2000})).WillOnce(Return(byte{0x02}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(registers.h, byte{0xff});
  EXPECT_EQ(registers.l, byte{0xfa});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
  EXPECT_EQ(registers.sp, word{0xfff8});
  EXPECT_EQ(registers.pc, word{0x2001});
  EXPECT_EQ(time_passed, cycles{8});
}

TEST(WordLoad, LD_0xc100_SP) {
  WordLoad instruction{
      IndirectWord::from(DoubleByteWord::from(ImmediateByte::instance(),
                                              ImmediateByte::instance())),
      WordRegister::sp()};

  CPU::RegisterSet registers{};
  registers.sp = word{0xfff8};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x0000})).WillOnce(Return(byte{0x00}));
  EXPECT_CALL(memory, read(word{0x0001})).WillOnce(Return(byte{0xc1}));
  EXPECT_CALL(memory, write(word{0xc100}, byte{0xf8}));
  EXPECT_CALL(memory, write(word{0xc101}, byte{0xff}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{16});
  EXPECT_EQ(registers.sp, word{0xfff8});
  EXPECT_EQ(registers.pc, word{0x0002});
}

TEST(ArithemeticInstruction, ADD_A_B) {
  ByteArithmeticOperation instruction{ByteRegister::a(), ByteRegister::b(),
                                      operations::add};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3a};
  registers.b = byte{0xc6};
  MockMemoryBus memory;

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{0});
  EXPECT_EQ(registers.a, byte{0x00});
  EXPECT_EQ(registers.b, byte{0xc6});
  EXPECT_TRUE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_TRUE(registers.f.carry);
}

TEST(ArithemeticInstruction, ADD_A_0xff) {
  ByteArithmeticOperation instruction{
      ByteRegister::a(), ImmediateByte::instance(), operations::add};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3c};
  registers.pc = word{0x4123};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x4123})).WillOnce(Return(byte{0xff}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x3b});
  EXPECT_EQ(registers.pc, word{0x4124});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_TRUE(registers.f.carry);
}

TEST(ArithemeticInstruction, ADD_A_HL) {
  ByteArithmeticOperation instruction{ByteRegister::a(),
                                      IndirectByte::from(WordRegister::hl()),
                                      operations::add};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3c};
  registers.h = byte{0x41};
  registers.l = byte{0x41};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x4141})).WillOnce(Return(byte{0x12}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x4e});
  EXPECT_EQ(registers.h, byte{0x41});
  EXPECT_EQ(registers.l, byte{0x41});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, ADC_A_E) {
  ByteArithmeticOperation instruction{ByteRegister::a(), ByteRegister::e(),
                                      operations::add_with_carry};

  CPU::RegisterSet registers{};
  registers.a = byte{0xe1};
  registers.e = byte{0x0f};
  registers.f.carry = true;
  MockMemoryBus memory;

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{0});
  EXPECT_EQ(registers.a, byte{0xf1});
  EXPECT_EQ(registers.e, byte{0x0f});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, ADC_A_0x3b) {
  ByteArithmeticOperation instruction{
      ByteRegister::a(), ImmediateByte::instance(), operations::add_with_carry};

  CPU::RegisterSet registers{};
  registers.a = byte{0xe1};
  registers.pc = word{0xe122};
  registers.f.carry = true;
  MockMemoryBus memory;

  EXPECT_CALL(memory, read(word{0xe122})).WillOnce(Return(byte{0x3b}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x1d});
  EXPECT_EQ(registers.pc, word{0xe123});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_TRUE(registers.f.carry);
}

TEST(ArithemeticInstruction, ADC_A_HL) {
  ByteArithmeticOperation instruction{ByteRegister::a(),
                                      IndirectByte::from(WordRegister::hl()),
                                      operations::add_with_carry};

  CPU::RegisterSet registers{};
  registers.a = byte{0xe1};
  registers.h = byte{0x2a};
  registers.l = byte{0x83};
  registers.f.carry = true;
  MockMemoryBus memory;

  EXPECT_CALL(memory, read(word{0x2a83})).WillOnce(Return(byte{0x1e}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x00});
  EXPECT_TRUE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_TRUE(registers.f.carry);
}

TEST(ArithemeticInstruction, SUB_E) {
  ByteArithmeticOperation instruction{ByteRegister::a(), ByteRegister::e(),
                                      operations::subtract};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3e};
  registers.e = byte{0x3e};
  MockMemoryBus memory;

  auto time_spend = instruction.execute(registers, memory);

  EXPECT_EQ(time_spend, cycles{});
  EXPECT_EQ(registers.a, byte{0x00});
  EXPECT_EQ(registers.e, byte{0x3e});
  EXPECT_TRUE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, SUB_0x0f) {
  ByteArithmeticOperation instruction{
      ByteRegister::a(), ImmediateByte::instance(), operations::subtract};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3e};
  registers.pc = word{0xa327};
  MockMemoryBus memory;

  EXPECT_CALL(memory, read(word{0xa327})).WillOnce(Return(byte{0x0f}));

  auto time_spend = instruction.execute(registers, memory);

  EXPECT_EQ(time_spend, cycles{4});
  EXPECT_EQ(registers.a, byte{0x2f});
  EXPECT_EQ(registers.pc, word{0xa328});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, SUB_HL) {
  ByteArithmeticOperation instruction{ByteRegister::a(),
                                      IndirectByte::from(WordRegister::hl()),
                                      operations::subtract};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3e};
  registers.h = byte{0x2a};
  registers.l = byte{0x83};
  MockMemoryBus memory;

  EXPECT_CALL(memory, read(word{0x2a83})).WillOnce(Return(byte{0x40}));

  auto time_spend = instruction.execute(registers, memory);

  EXPECT_EQ(time_spend, cycles{4});
  EXPECT_EQ(registers.a, byte{0xfe});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_TRUE(registers.f.carry);
}

TEST(ArithemeticInstruction, SBC_H) {
  ByteArithmeticOperation instruction{ByteRegister::a(), ByteRegister::h(),
                                      operations::subtract_with_carry};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3b};
  registers.h = byte{0x2a};
  registers.f.carry = true;
  MockMemoryBus memory;

  auto time_spend = instruction.execute(registers, memory);

  EXPECT_EQ(time_spend, cycles{});
  EXPECT_EQ(registers.a, byte{0x10});
  EXPECT_EQ(registers.h, byte{0x2a});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, SBC_0x3a) {
  ByteArithmeticOperation instruction{ByteRegister::a(),
                                      ImmediateByte::instance(),
                                      operations::subtract_with_carry};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3b};
  registers.pc = word{0x8ba2};
  registers.f.carry = true;
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x8ba2})).WillOnce(Return(byte{0x3a}));

  auto time_spend = instruction.execute(registers, memory);

  EXPECT_EQ(time_spend, cycles{4});
  EXPECT_EQ(registers.a, byte{0x00});
  EXPECT_EQ(registers.pc, word{0x8ba3});
  EXPECT_TRUE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, SBC_HL) {
  ByteArithmeticOperation instruction{ByteRegister::a(),
                                      IndirectByte::from(WordRegister::hl()),
                                      operations::subtract_with_carry};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3b};
  registers.h = byte{0x2a};
  registers.l = byte{0xbb};
  registers.f.carry = true;
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x2abb})).WillOnce(Return(byte{0x4f}));

  auto time_spend = instruction.execute(registers, memory);

  EXPECT_EQ(time_spend, cycles{4});
  EXPECT_EQ(registers.a, byte{0xeb});
  EXPECT_EQ(registers.h, byte{0x2a});
  EXPECT_EQ(registers.l, byte{0xbb});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_TRUE(registers.f.carry);
}

TEST(ArithemeticInstruction, AND_L) {
  ByteArithmeticOperation instruction{ByteRegister::a(), ByteRegister::l(),
                                      operations::bitwise_and};

  CPU::RegisterSet registers{};
  registers.a = byte{0x5a};
  registers.l = byte{0x3f};
  MockMemoryBus memory;

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{0});
  EXPECT_EQ(registers.a, byte{0x1a});
  EXPECT_EQ(registers.l, byte{0x3f});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, AND_0x18) {
  ByteArithmeticOperation instruction{
      ByteRegister::a(), ImmediateByte::instance(), operations::bitwise_and};

  CPU::RegisterSet registers{};
  registers.a = byte{0x5a};
  registers.pc = word{0x3f00};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x3f00})).WillOnce(Return(byte{0x18}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x18});
  EXPECT_EQ(registers.pc, word{0x3f01});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, AND_HL) {
  ByteArithmeticOperation instruction{ByteRegister::a(),
                                      IndirectByte::from(WordRegister::hl()),
                                      operations::bitwise_and};

  CPU::RegisterSet registers{};
  registers.a = byte{0x5a};
  registers.h = byte{0xaf};
  registers.l = byte{0x3f};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0xaf3f})).WillOnce(Return(byte{0x00}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x00});
  EXPECT_EQ(registers.h, byte{0xaf});
  EXPECT_EQ(registers.l, byte{0x3f});
  EXPECT_TRUE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, OR_A) {
  ByteArithmeticOperation instruction{ByteRegister::a(), ByteRegister::a(),
                                      operations::bitwise_or};

  CPU::RegisterSet registers{};
  registers.a = byte{0x5a};
  MockMemoryBus memory;

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{0});
  EXPECT_EQ(registers.a, byte{0x5a});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, OR_0x03) {
  ByteArithmeticOperation instruction{
      ByteRegister::a(), ImmediateByte::instance(), operations::bitwise_or};

  CPU::RegisterSet registers{};
  registers.a = byte{0x5a};
  registers.pc = word{0x0150};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x0150})).WillOnce(Return(byte{0x03}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x5b});
  EXPECT_EQ(registers.pc, word{0x0151});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, OR_HL) {
  ByteArithmeticOperation instruction{ByteRegister::a(),
                                      IndirectByte::from(WordRegister::hl()),
                                      operations::bitwise_or};

  CPU::RegisterSet registers{};
  registers.a = byte{0x5a};
  registers.h = byte{0x20};
  registers.l = byte{0x00};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x2000})).WillOnce(Return(byte{0x0f}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x5f});
  EXPECT_EQ(registers.h, byte{0x20});
  EXPECT_EQ(registers.l, byte{0x00});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, XOR_A) {
  ByteArithmeticOperation instruction{ByteRegister::a(), ByteRegister::a(),
                                      operations::bitwise_xor};

  CPU::RegisterSet registers{};
  registers.a = byte{0xff};
  MockMemoryBus memory;

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{0});
  EXPECT_EQ(registers.a, byte{0x00});
  EXPECT_TRUE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, XOR_0x0f) {
  ByteArithmeticOperation instruction{
      ByteRegister::a(), ImmediateByte::instance(), operations::bitwise_xor};

  CPU::RegisterSet registers{};
  registers.a = byte{0xff};
  registers.pc = word{0x48a0};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x48a0})).WillOnce(Return(byte{0x0f}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0xf0});
  EXPECT_EQ(registers.pc, word{0x48a1});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, XOR_HL) {
  ByteArithmeticOperation instruction{ByteRegister::a(),
                                      IndirectByte::from(WordRegister::hl()),
                                      operations::bitwise_xor};

  CPU::RegisterSet registers{};
  registers.a = byte{0xff};
  registers.h = byte{0x89};
  registers.l = byte{0x18};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x8918})).WillOnce(Return(byte{0x8a}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x75});
  EXPECT_EQ(registers.h, byte{0x89});
  EXPECT_EQ(registers.l, byte{0x18});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, CP_B) {
  ByteArithmeticOperation instruction{ByteRegister::a(), ByteRegister::b(),
                                      operations::compare};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3c};
  registers.b = byte{0x2f};
  MockMemoryBus memory;

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{0});
  EXPECT_EQ(registers.a, byte{0x3c});
  EXPECT_EQ(registers.b, byte{0x2f});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, CP_0x3c) {
  ByteArithmeticOperation instruction{
      ByteRegister::a(), ImmediateByte::instance(), operations::compare};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3c};
  registers.pc = word{0x221f};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x221f})).WillOnce(Return(byte{0x3c}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x3c});
  EXPECT_EQ(registers.pc, word{0x2220});
  EXPECT_TRUE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}

TEST(ArithemeticInstruction, CP_HL) {
  ByteArithmeticOperation instruction{ByteRegister::a(),
                                      IndirectByte::from(WordRegister::hl()),
                                      operations::compare};

  CPU::RegisterSet registers{};
  registers.a = byte{0x3c};
  registers.h = byte{0x94};
  registers.l = byte{0x4e};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x944e})).WillOnce(Return(byte{0x40}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers.a, byte{0x3c});
  EXPECT_EQ(registers.h, byte{0x94});
  EXPECT_EQ(registers.l, byte{0x4e});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_TRUE(registers.f.carry);
}

TEST(ArithemeticInstruction, INC_A) {
  ByteArithmeticOperation instruction{
      ByteRegister::a(), ConstantByte::from(byte{1}), operations::increment};

  CPU::RegisterSet registers{};
  registers.a = byte{0xff};
  MockMemoryBus memory;

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{0});
  EXPECT_EQ(registers.a, byte{0x00});
  EXPECT_TRUE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}
TEST(ArithemeticInstruction, INC_HL) {
  ByteArithmeticOperation instruction{IndirectByte::from(WordRegister::hl()),
                                      ConstantByte::from(byte{1}),
                                      operations::increment};

  CPU::RegisterSet registers{};
  registers.h = byte{0x40};
  registers.l = byte{0x00};
  MockMemoryBus memory;

  EXPECT_CALL(memory, read(word{0x4000})).WillOnce(Return(byte{0x50}));
  EXPECT_CALL(memory, write(word{0x4000}, byte{0x51}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_FALSE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}
TEST(ArithemeticInstruction, DEC_L) {
  ByteArithmeticOperation instruction{
      ByteRegister::l(), ConstantByte::from(byte{1}), operations::decrement};

  CPU::RegisterSet registers{};
  registers.l = byte{0x01};
  MockMemoryBus memory;

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{0});
  EXPECT_EQ(registers.l, byte{0x00});
  EXPECT_TRUE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_FALSE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}
TEST(ArithemeticInstruction, DEC_HL) {
  ByteArithmeticOperation instruction{IndirectByte::from(WordRegister::hl()),
                                      ConstantByte::from(byte{1}),
                                      operations::decrement};

  CPU::RegisterSet registers{};
  registers.h = byte{0x40};
  registers.l = byte{0x00};
  MockMemoryBus memory;

  EXPECT_CALL(memory, read(word{0x4000})).WillOnce(Return(byte{0x00}));
  EXPECT_CALL(memory, write(word{0x4000}, byte{0xff}));

  auto time_passed = instruction.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_FALSE(registers.f.zero);
  EXPECT_TRUE(registers.f.negate);
  EXPECT_TRUE(registers.f.half_carry);
  EXPECT_FALSE(registers.f.carry);
}
} // namespace