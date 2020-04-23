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

namespace {
using namespace greenboy;
using namespace data_access;
using ::testing::_;
using ::testing::Return;

TEST(ConstantByte, ReadsTheConstantValue) {
  ConstantByte access{byte{0x0a}};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_EQ(access.read(registers, memory), byte{0x0a});
}

TEST(ConstantByte, RejectsWrites) {
  ConstantByte access{byte{0x0a}};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_THROW(access.write(registers, memory, byte{0x34}), std::runtime_error);
}

TEST(ByteRegister, ReadsFromRegister) {
  ByteRegister access{CPU::R8::B};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.b = byte{0x12};

  EXPECT_EQ(access.read(registers, memory), byte{0x12});
}

TEST(ByteRegister, WritesToRegister) {
  ByteRegister access{CPU::R8::C};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.c = byte{0x12};

  access.write(registers, memory, byte{0x34});
  EXPECT_EQ(registers.c, byte{0x34});
}

TEST(ByteRegister, ReadsFromInvalidRegistersThrow) {
  ByteRegister access{(CPU::R8)-1};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_THROW(access.read(registers, memory), std::runtime_error);
}

TEST(ByteRegister, WritesToInvalidRegistersThrow) {
  ByteRegister access{(CPU::R8)-1};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_THROW(access.write(registers, memory, byte{0x34}), std::runtime_error);
}

TEST(ImmediateByte, ReadsTheNextByte) {
  ImmediateByte access;
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.pc = word{0x0100};
  EXPECT_CALL(memory, read(word{0x0100})).WillOnce(Return(byte{0x54}));

  EXPECT_EQ(access.read(registers, memory), byte{0x54});
}

TEST(ImmediateByte, ReadsIncrementTheProgramCounter) {
  ImmediateByte access;
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.pc = word{0x0100};

  access.read(registers, memory);

  EXPECT_EQ(registers.pc, word{0x0101});
}

TEST(ImmediateByte, RejectsWrites) {
  ImmediateByte access;
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_THROW(access.write(registers, memory, byte{0x34}), std::runtime_error);
}

TEST(IndirectByte, ReadsAreDelegatedToMemoryLocationsDictatedByThePointer) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectByte access{std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillOnce(Return(word{0x1234}));
  EXPECT_CALL(memory, read(word{0x1234})).WillOnce(Return(byte{0xde}));

  EXPECT_EQ(access.read(registers, memory), byte{0xde});
}

TEST(IndirectByte, WritesAreDelegatedToMemoryLocationsDictatedByThePointer) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectByte access{std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillOnce(Return(word{0x1234}));
  EXPECT_CALL(memory, write(word{0x1234}, byte{0xde}));

  access.write(registers, memory, byte{0xde});
}

TEST(IncrementingWord, ValueIsIncrementedAfterRead) {
  auto pointer = std::make_shared<MockWordAccess>();
  IncrementingWord access{std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillRepeatedly(Return(word{0x1234}));
  EXPECT_CALL(*pointer, write(_, _, word{0x1235}));

  access.read(registers, memory);
}

TEST(IncrementingWord, WritesAreRejected) {
  auto pointer = std::make_shared<MockWordAccess>();
  IncrementingWord access{std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_THROW(access.write(registers, memory, word{0xfaaa}),
               std::runtime_error);
}

TEST(DecrementingWord, PointerIsDecrementedAfterRead) {
  auto pointer = std::make_shared<MockWordAccess>();
  DecrementingWord access{std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillRepeatedly(Return(word{0x1234}));
  EXPECT_CALL(*pointer, write(_, _, word{0x1233}));

  access.read(registers, memory);
}

TEST(DecrementingWord, WritesAreRejected) {
  auto pointer = std::make_shared<MockWordAccess>();
  DecrementingWord access{std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_THROW(access.write(registers, memory, word{0xffaa}),
               std::runtime_error);
}

TEST(WordRegister, ReadsFromRegister) {
  WordRegister access{CPU::R16::AF};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.a = byte{0x12};
  registers.f = CPU::Flags{byte{0x43}};

  EXPECT_EQ(access.read(registers, memory), word{0x1240});
}

TEST(WordRegister, WritesToRegister) {
  WordRegister access{CPU::R16::DE};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  access.write(registers, memory, word{0x3465});
  EXPECT_EQ(registers.d, byte{0x34});
  EXPECT_EQ(registers.e, byte{0x65});
}

TEST(WordRegister, ReadsFromInvalidRegisterThrow) {
  WordRegister access{(CPU::R16)-1};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.a = byte{0x12};
  registers.f = CPU::Flags{byte{0x43}};

  EXPECT_THROW(access.read(registers, memory), std::runtime_error);
}

TEST(WordRegister, WritesToInvalidRegisterThrow) {
  WordRegister access{(CPU::R16)-1};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_THROW(access.write(registers, memory, word{0x3465}),
               std::runtime_error);
}

TEST(DoubleByteWord, DelegateReadsToByteAccesses) {
  auto high = std::make_shared<MockByteAccess>();
  auto low = std::make_shared<MockByteAccess>();
  DoubleByteWord access(high, low);
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*high, read(_, _)).WillOnce(Return(byte{0xde}));
  EXPECT_CALL(*low, read(_, _)).WillOnce(Return(byte{0x7b}));

  EXPECT_EQ(access.read(registers, memory), word{0xde7b});
}

TEST(DoubleByteWord, DelegateWritesToByteAccesses) {
  auto high = std::make_shared<MockByteAccess>();
  auto low = std::make_shared<MockByteAccess>();
  DoubleByteWord access(high, low);
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*high, write(_, _, byte{0x05}));
  EXPECT_CALL(*low, write(_, _, byte{0x31}));

  access.write(registers, memory, word{0x0531});
}

TEST(IndirectWord, ReadsAreDelegatedToMemoryLocationsDictatedByThePointer) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectWord access{std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillOnce(Return(word{0x3762}));
  EXPECT_CALL(memory, read(word{0x3762})).WillOnce(Return(byte{0xaa}));
  EXPECT_CALL(memory, read(word{0x3763})).WillOnce(Return(byte{0xbb}));

  EXPECT_EQ(access.read(registers, memory), word{0xbbaa});
}

TEST(IndirectWord, WritesAreDelegatedToMemoryLocationsDictatedByThePointer) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectWord access{std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillOnce(Return(word{0x1234}));
  EXPECT_CALL(memory, write(word{0x1234}, byte{0xde}));
  EXPECT_CALL(memory, write(word{0x1235}, byte{0xed}));

  access.write(registers, memory, word{0xedde});
}

} // namespace