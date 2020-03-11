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

  CPU::RegisterSet expected_register_state;
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

  CPU::RegisterSet expected_register_state;
  expected_register_state.pc = word{0x3020};
  expected_register_state.sp = word{0xfffc};
  EXPECT_EQ(time_passed, cycles{24});
  EXPECT_EQ(registers, expected_register_state);
}
} // namespace