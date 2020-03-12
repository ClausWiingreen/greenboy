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
  MOCK_METHOD(byte, read, (word), (override));
  MOCK_METHOD(void, write, (word, byte), (override));
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

TEST(Instruction, LOAD_R8_R8) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x3020};
  registers.b = byte{0x30};
  registers.c = byte{0x80};

  MockMemoryBus memory;

  auto time_passed = LOAD_R8_R8<R8::B, R8::C>{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x3021};
  expected_register_state.b = byte{0x80};
  expected_register_state.c = byte{0x80};
  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, LOAD_HL_R8) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x3020};
  registers.b = byte{0x30};
  registers.h = byte{0x20};
  registers.l = byte{0x12};

  MockMemoryBus memory;
  EXPECT_CALL(memory, write(word{0x2012}, byte{0x30}));

  auto time_passed = LOAD_HL_R8<R8::B>{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x3021};
  expected_register_state.b = byte{0x30};
  expected_register_state.h = byte{0x20};
  expected_register_state.l = byte{0x12};
  EXPECT_EQ(time_passed, cycles{8});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, LOAD_R8_HL) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x3020};
  registers.b = byte{0x62};
  registers.h = byte{0x2c};
  registers.l = byte{0x72};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x2c72})).WillOnce(Return(byte{0x3b}));

  auto time_passed = LOAD_R8_HL<R8::B>{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x3021};
  expected_register_state.b = byte{0x3b};
  expected_register_state.h = byte{0x2c};
  expected_register_state.l = byte{0x72};
  EXPECT_EQ(time_passed, cycles{8});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, LOAD_R8_n) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x3020};
  registers.b = byte{0x30};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x3021})).WillOnce(Return(byte{0x31}));

  auto time_passed = LOAD_R8_n<R8::B>{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x3021};
  expected_register_state.b = byte{0x31};
  EXPECT_EQ(time_passed, cycles{8});
  EXPECT_EQ(registers, expected_register_state);
}

TEST(Instruction, LOAD_HL_n) {
  CPU::RegisterSet registers{};
  registers.pc = word{0x3020};
  registers.b = byte{0x30};
  registers.h = byte{0x62};
  registers.l = byte{0xa5};

  MockMemoryBus memory;
  EXPECT_CALL(memory, read(word{0x3021})).WillOnce(Return(byte{0x88}));
  EXPECT_CALL(memory, write(word{0x62a5}, byte{0x88}));

  auto time_passed = LOAD_HL_n{}.execute(registers, memory);

  CPU::RegisterSet expected_register_state{};
  expected_register_state.pc = word{0x3022};
  expected_register_state.b = byte{0x30};
  expected_register_state.h = byte{0x62};
  expected_register_state.l = byte{0xa5};
  EXPECT_EQ(time_passed, cycles{12});
  EXPECT_EQ(registers, expected_register_state);
}
} // namespace