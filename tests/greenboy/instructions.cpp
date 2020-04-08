#include "greenboy/instruction.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "mocks/word_access.hpp"

namespace {
using namespace greenboy;
using namespace instructions;
using ::testing::_;
using ::testing::Return;

class MockMemoryBus : public MemoryBus {
public:
  MOCK_METHOD(byte, read, (word), (const, override));
  MOCK_METHOD(void, write, (word, byte), (override));
};

class MockByteAccess : public ByteAccess {
public:
  MOCK_METHOD(byte, read, (CPU::RegisterSet &, MemoryBus &), (const, override));
  MOCK_METHOD(void, write, (CPU::RegisterSet &, MemoryBus &, byte), (override));
};

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

TEST(ConstantByteAccess, ReadsTheConstantValue) {
  ConstantByteAccess access{byte{0x0a}};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_EQ(access.read(registers, memory), byte{0x0a});
}

TEST(ConstantByteAccess, RejectsWrites) {
  ConstantByteAccess access{byte{0x0a}};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_THROW(access.write(registers, memory, byte{0x34}), std::runtime_error);
}

TEST(ByteRegisterAccess, ReadsFromRegister) {
  ByteRegisterAccess access{CPU::R8::B};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.b = byte{0x12};

  EXPECT_EQ(access.read(registers, memory), byte{0x12});
}

TEST(ByteRegisterAccess, WritesToRegister) {
  ByteRegisterAccess access{CPU::R8::C};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.c = byte{0x12};

  access.write(registers, memory, byte{0x34});
  EXPECT_EQ(registers.c, byte{0x34});
}

TEST(ImmediateByteAccess, ReadsTheNextByte) {
  ImmediateByteAccess access;
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.pc = word{0x0100};
  EXPECT_CALL(memory, read(word{0x0100})).WillOnce(Return(byte{0x54}));

  EXPECT_EQ(access.read(registers, memory), byte{0x54});
}

TEST(ImmediateByteAccess, ReadsIncrementTheProgramCounter) {
  ImmediateByteAccess access;
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.pc = word{0x0100};

  access.read(registers, memory);

  EXPECT_EQ(registers.pc, word{0x0101});
}

TEST(ImmediateByteAccess, RejectsWrites) {
  ImmediateByteAccess access;
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_THROW(access.write(registers, memory, byte{0x34}), std::runtime_error);
}

TEST(IndirectByteAccess,
     ReadsAreDelegatedToMemoryLocationsDictatedByThePointer) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectByteAccess access{std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillOnce(Return(word{0x1234}));
  EXPECT_CALL(memory, read(word{0x1234})).WillOnce(Return(byte{0xde}));

  EXPECT_EQ(access.read(registers, memory), byte{0xde});
}

TEST(IndirectByteAccess,
     WritesAreDelegatedToMemoryLocationsDictatedByThePointer) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectByteAccess access{std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillOnce(Return(word{0x1234}));
  EXPECT_CALL(memory, write(word{0x1234}, byte{0xde}));

  access.write(registers, memory, byte{0xde});
}

TEST(IndirectAndIncrementByteAccess,
     ReadsAreDelegatedToMemoryLocationsDictatedByThePointer) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectAndIncrementByteAccess access{
      std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillRepeatedly(Return(word{0x1234}));
  EXPECT_CALL(memory, read(word{0x1234})).WillOnce(Return(byte{0xde}));

  EXPECT_EQ(access.read(registers, memory), byte{0xde});
}

TEST(IndirectAndIncrementByteAccess,
     WritesAreDelegatedToMemoryLocationsDictatedByThePointer) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectAndIncrementByteAccess access{
      std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillRepeatedly(Return(word{0x1234}));
  EXPECT_CALL(memory, write(word{0x1234}, byte{0xde}));

  access.write(registers, memory, byte{0xde});
}

TEST(IndirectAndIncrementByteAccess, PointerIsIncrementedAfterRead) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectAndIncrementByteAccess access{
      std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillRepeatedly(Return(word{0x1234}));
  EXPECT_CALL(memory, read(word{0x1234}));
  EXPECT_CALL(*pointer, write(_, _, word{0x1235}));

  access.read(registers, memory);
}

TEST(IndirectAndIncrementByteAccess, PointerIsIncrementedAfterWrite) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectAndIncrementByteAccess access{
      std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillRepeatedly(Return(word{0x1234}));
  EXPECT_CALL(memory, write(word{0x1234}, _));
  EXPECT_CALL(*pointer, write(_, _, word{0x1235}));

  access.write(registers, memory, byte{0xfa});
}

TEST(IndirectAndDecrementByteAccess,
     ReadsAreDelegatedToMemoryLocationsDictatedByThePointer) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectAndDecrementByteAccess access{
      std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillRepeatedly(Return(word{0x1234}));
  EXPECT_CALL(memory, read(word{0x1234})).WillOnce(Return(byte{0xde}));

  EXPECT_EQ(access.read(registers, memory), byte{0xde});
}

TEST(IndirectAndDecrementByteAccess,
     WritesAreDelegatedToMemoryLocationsDictatedByThePointer) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectAndDecrementByteAccess access{
      std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillRepeatedly(Return(word{0x1234}));
  EXPECT_CALL(memory, write(word{0x1234}, byte{0xde}));

  access.write(registers, memory, byte{0xde});
}

TEST(IndirectAndDecrementByteAccess, PointerIsDecrementedAfterRead) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectAndDecrementByteAccess access{
      std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillRepeatedly(Return(word{0x1234}));
  EXPECT_CALL(memory, read(word{0x1234}));
  EXPECT_CALL(*pointer, write(_, _, word{0x1233}));

  access.read(registers, memory);
}

TEST(IndirectAndDecrementByteAccess, PointerIsIncrementedAfterWrite) {
  auto pointer = std::make_shared<MockWordAccess>();
  IndirectAndDecrementByteAccess access{
      std::static_pointer_cast<WordAccess>(pointer)};

  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_CALL(*pointer, read(_, _)).WillRepeatedly(Return(word{0x1234}));
  EXPECT_CALL(memory, write(word{0x1234}, _));
  EXPECT_CALL(*pointer, write(_, _, word{0x1233}));

  access.write(registers, memory, byte{0xfa});
}

TEST(WordRegisterAccess, ReadsFromRegister) {
  WordRegisterAccess access{CPU::R16::BC};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};
  registers.b = byte{0x12};
  registers.c = byte{0x43};

  EXPECT_EQ(access.read(registers, memory), word{0x1243});
}

TEST(WordRegisterAccess, WritesToRegister) {
  WordRegisterAccess access{CPU::R16::DE};
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  access.write(registers, memory, word{0x3465});
  EXPECT_EQ(registers.d, byte{0x34});
  EXPECT_EQ(registers.e, byte{0x65});
}

} // namespace