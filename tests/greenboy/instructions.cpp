#include "greenboy/instruction.hpp"
#include "mocks/byte_access.hpp"
#include "mocks/memory_bus.hpp"
#include "mocks/word_access.hpp"
#include "gtest/gtest.h"

namespace {
using namespace greenboy;
using namespace instructions;
using namespace data_access;
using ::testing::_;
using ::testing::Return;

TEST(ByteLoad, RejectsNullPointers) {
  auto mocked_access = std::make_shared<MockByteAccess>();

  EXPECT_THROW(ByteLoad(nullptr, nullptr), std::invalid_argument);
  EXPECT_THROW(ByteLoad(mocked_access, nullptr), std::invalid_argument);
  EXPECT_THROW(ByteLoad(nullptr, mocked_access), std::invalid_argument);
  EXPECT_NO_THROW(ByteLoad(mocked_access, mocked_access));
}

TEST(ByteLoad, TakesTheReadValueFromSourceAndWritesItToDestination) {
  auto source = std::make_shared<MockByteAccess>();
  auto destination = std::make_shared<MockByteAccess>();
  CPU::RegisterSet registers{};
  MockMemoryBus memory;

  EXPECT_CALL(*source, read(_, _)).WillOnce(Return(byte{0x24}));
  EXPECT_CALL(*destination, write(_, _, byte{0x24}));

  ByteLoad{destination, source}.execute(registers, memory);
}

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

TEST(WordLoad, RejectsNullPointers) {
  auto access = std::make_shared<MockWordAccess>();
  EXPECT_THROW(WordLoad(nullptr, nullptr), std::invalid_argument);
  EXPECT_THROW(WordLoad(access, nullptr), std::invalid_argument);
  EXPECT_THROW(WordLoad(nullptr, access), std::invalid_argument);
  EXPECT_NO_THROW(WordLoad(access, access));
}

TEST(WordLoad, TakesTheReadValueFromSourceAndWritesItToDestination) {
  auto source = std::make_shared<MockWordAccess>();
  auto destination = std::make_shared<MockWordAccess>();
  CPU::RegisterSet registers{};
  MockMemoryBus memory;

  EXPECT_CALL(*source, read(_, _)).WillOnce(Return(word{0x2480}));
  EXPECT_CALL(*destination, write(_, _, word{0x2480}));

  WordLoad{destination, source}.execute(registers, memory);
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
  /*
   */
}
} // namespace