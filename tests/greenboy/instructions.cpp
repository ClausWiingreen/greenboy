#include "greenboy/instruction.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

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
  MOCK_METHOD(byte, read, (CPU::RegisterSet &, const MemoryBus &),
              (const, override));
  MOCK_METHOD(void, write, (CPU::RegisterSet &, MemoryBus &, byte), (override));
};

TEST(Instruction, NOP) {
  CPU::RegisterSet registers{};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(_)).Times(0);
  EXPECT_CALL(memory, write(_, _)).Times(0);

  auto time_passed = NOP{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, CALL) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x0100};
  registers.sp = word{0xfffe};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x0101})).WillOnce(Return(byte{0x20}));
  EXPECT_CALL(memory, read(word{0x0102})).WillOnce(Return(byte{0x30}));
  EXPECT_CALL(memory, write(word{0xfffe}, byte{0x01}));
  EXPECT_CALL(memory, write(word{0xfffd}, byte{0x03}));

  auto time_passed = CALL{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x3020};
  expected_register_state.sp = word{0xfffc};
  EXPECT_EQ(time_passed, cycles{24});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, RET) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x3020};
  registers.sp = word{0xfffc};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0xfffe})).WillOnce(Return(byte{0x01}));
  EXPECT_CALL(memory, read(word{0xfffd})).WillOnce(Return(byte{0x03}));

  auto time_passed = RET{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x0103};
  expected_register_state.sp = word{0xfffe};
  EXPECT_EQ(time_passed, cycles{16});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, SET) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x3020};
  registers.b = byte{0x30};

  MockMemoryBus memory;

  auto time_passed = SET{0, CPU::R8::B}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x3022};
  expected_register_state.b = byte{0x31};
  EXPECT_EQ(time_passed, cycles{8});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, RES) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x3020};
  registers.b = byte{0x31};

  MockMemoryBus memory;

  auto time_passed = RES{0, CPU::R8::B}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x3022};
  expected_register_state.b = byte{0x30};
  EXPECT_EQ(time_passed, cycles{8});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, LOAD_A_HLI) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x305d};
  registers.h = byte{0x5b};
  registers.l = byte{0x0c};
  registers.a = byte{0x0a};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x5b0c})).WillOnce(Return(byte{0x37}));

  auto time_passed = LOAD_A_HLI{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x305e};
  expected_register_state.h = byte{0x5b};
  expected_register_state.l = byte{0x0d};
  expected_register_state.a = byte{0x37};

  EXPECT_EQ(time_passed, cycles{8});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, LOAD_A_HLD) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x305d};
  registers.h = byte{0x5b};
  registers.l = byte{0x0e};
  registers.a = byte{0x0a};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x5b0e})).WillOnce(Return(byte{0x37}));

  auto time_passed = LOAD_A_HLD{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x305e};
  expected_register_state.h = byte{0x5b};
  expected_register_state.l = byte{0x0d};
  expected_register_state.a = byte{0x37};

  EXPECT_EQ(time_passed, cycles{8});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, LOAD_HLI_A) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x305d};
  registers.h = byte{0x5b};
  registers.l = byte{0x0c};
  registers.a = byte{0x0a};

  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0x5b0c}, byte{0x0a}));

  auto time_passed = LOAD_HLI_A{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x305e};
  expected_register_state.h = byte{0x5b};
  expected_register_state.l = byte{0x0d};
  expected_register_state.a = byte{0x0a};

  EXPECT_EQ(time_passed, cycles{8});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, LOAD_HLD_A) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x305d};
  registers.h = byte{0x5b};
  registers.l = byte{0x0e};
  registers.a = byte{0x0a};

  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0x5b0e}, byte{0x0a}));

  auto time_passed = LOAD_HLD_A{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x305e};
  expected_register_state.h = byte{0x5b};
  expected_register_state.l = byte{0x0d};
  expected_register_state.a = byte{0x0a};

  EXPECT_EQ(time_passed, cycles{8});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, RST) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x0103};
  registers.sp = word{0xfffe};
  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0xfffe}, byte{0x01}));
  EXPECT_CALL(memory, write(word{0xfffd}, byte{0x04}));

  auto time_passed = RST{word{0x0008}}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x0008};
  expected_register_state.sp = word{0xfffc};
  EXPECT_EQ(time_passed, cycles{16});
  EXPECT_EQ(registers, expected_register_state);
}

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

TEST(ImmediateByteAccess, RejectsWrites) {
  ImmediateByteAccess access;
  MockMemoryBus memory;
  CPU::RegisterSet registers{};

  EXPECT_THROW(access.write(registers, memory, byte{0x34}), std::runtime_error);
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