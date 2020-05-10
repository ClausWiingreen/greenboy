#include "greenboy/instruction.hpp"
#include "mocks/byte_access.hpp"
#include "mocks/memory_bus.hpp"
#include "mocks/word_access.hpp"
#include "gtest/gtest.h"

#include "greenboy/instructions/byte_arithmetic_operation.hpp"
#include "greenboy/instructions/byte_load.hpp"
#include "greenboy/instructions/word_load.hpp"

namespace {
using namespace greenboy;
using namespace instructions;
using namespace data_access;
using ::testing::_;
using ::testing::Return;

TEST(ByteLoad, TakesTheReadValueFromSourceAndWritesItToDestination) {
  auto source = std::make_shared<MockByteAccess>();
  auto destination = std::make_shared<MockByteAccess>();
  CPU::RegisterSet registers{};
  MockMemoryBus memory;

  EXPECT_CALL(*source, read(_, _)).WillOnce(Return(byte{0x24}));
  EXPECT_CALL(*destination, write(_, _, byte{0x24}));

  ByteLoad{destination, source}.execute(registers, memory);
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

TEST(ByteArithmeticOperation,
     AppliesTheOperationToTheOperandsAndAssignsTheValue) {
  auto source = std::make_shared<MockByteAccess>();
  auto destination = std::make_shared<MockByteAccess>();
  CPU::RegisterSet registers{};
  MockMemoryBus memory;

  EXPECT_CALL(*source, read(_, _)).WillOnce(Return(byte{0x24}));
  EXPECT_CALL(*destination, read(_, _)).WillOnce(Return(byte{0x00}));
  EXPECT_CALL(*destination, write(_, _, byte{0x80}));

  int times_called = 0;
  auto operation = [&times_called](auto lhs, auto rhs, auto &f) {
    times_called++;
    EXPECT_EQ(lhs, byte{0x00});
    EXPECT_EQ(rhs, byte{0x24});
    return byte{0x80};
  };

  ByteArithmeticOperation{destination, source, operation}.execute(registers,
                                                                  memory);
  EXPECT_EQ(times_called, 1);
}

TEST(ByteArithmeticOperation, OperationAdd) {
  byte lhs{0x00};
  byte rhs{0x00};
  CPU::Flags f{};

  auto result = operations::add(lhs, rhs, f);

  EXPECT_EQ(result, byte{0x00});
  EXPECT_TRUE(f.zero);
  EXPECT_FALSE(f.negate);
  EXPECT_FALSE(f.half_carry);
  EXPECT_FALSE(f.carry);
}
TEST(ByteArithmeticOperation, OperationAddWithCarry) {
  byte lhs{0x01};
  byte rhs{0x0f};
  CPU::Flags f{};
  f.carry = true;

  auto result = operations::add_with_carry(lhs, rhs, f);

  EXPECT_EQ(result, byte{0x11});
  EXPECT_FALSE(f.zero);
  EXPECT_FALSE(f.negate);
  EXPECT_TRUE(f.half_carry);
  EXPECT_FALSE(f.carry);
}
TEST(ByteArithmeticOperation, OperationSubtract) {
  byte lhs{0x12};
  byte rhs{0x0e};
  CPU::Flags f{};

  auto result = operations::subtract(lhs, rhs, f);

  EXPECT_EQ(result, byte{0x04});
  EXPECT_FALSE(f.zero);
  EXPECT_TRUE(f.negate);
  EXPECT_TRUE(f.half_carry);
  EXPECT_FALSE(f.carry);
}
TEST(ByteArithmeticOperation, OperationSubtractWithCarry) {
  byte lhs{0x22};
  byte rhs{0x0e};
  CPU::Flags f{};
  f.carry = true;

  auto result = operations::subtract_with_carry(lhs, rhs, f);

  EXPECT_EQ(result, byte{0x13});
  EXPECT_FALSE(f.zero);
  EXPECT_TRUE(f.negate);
  EXPECT_TRUE(f.half_carry);
  EXPECT_FALSE(f.carry);
}
TEST(ByteArithmeticOperation, OperationAnd) {
  byte lhs{0x22};
  byte rhs{0x0e};
  CPU::Flags f{};

  auto result = operations::bitwise_and(lhs, rhs, f);

  EXPECT_EQ(result, byte{0x02});
  EXPECT_FALSE(f.zero);
  EXPECT_FALSE(f.negate);
  EXPECT_TRUE(f.half_carry);
  EXPECT_FALSE(f.carry);
}
TEST(ByteArithmeticOperation, OperationOr) {
  byte lhs{0x22};
  byte rhs{0x0e};
  CPU::Flags f{};

  auto result = operations::bitwise_or(lhs, rhs, f);

  EXPECT_EQ(result, byte{0x2e});
  EXPECT_FALSE(f.zero);
  EXPECT_FALSE(f.negate);
  EXPECT_FALSE(f.half_carry);
  EXPECT_FALSE(f.carry);
}
TEST(ByteArithmeticOperation, OperationXor) {
  byte lhs{0x22};
  byte rhs{0x0e};
  CPU::Flags f{};

  auto result = operations::bitwise_xor(lhs, rhs, f);

  EXPECT_EQ(result, byte{0x2c});
  EXPECT_FALSE(f.zero);
  EXPECT_FALSE(f.negate);
  EXPECT_FALSE(f.half_carry);
  EXPECT_FALSE(f.carry);
}
TEST(ByteArithmeticOperation, OperationCompare) {
  byte lhs{0x22};
  byte rhs{0x0e};
  CPU::Flags f{};

  auto result = operations::compare(lhs, rhs, f);

  EXPECT_EQ(result, byte{0x22});
  EXPECT_FALSE(f.zero);
  EXPECT_TRUE(f.negate);
  EXPECT_TRUE(f.half_carry);
  EXPECT_FALSE(f.carry);
}
TEST(ByteArithmeticOperation, OperationIncrement) {
  byte lhs{0x22};
  byte rhs{0x01};
  CPU::Flags f{};

  auto result = operations::increment(lhs, rhs, f);

  EXPECT_EQ(result, byte{0x23});
  EXPECT_FALSE(f.zero);
  EXPECT_FALSE(f.negate);
  EXPECT_FALSE(f.half_carry);
  EXPECT_FALSE(f.carry);
}
TEST(ByteArithmeticOperation, OperationDecrement) {
  byte lhs{0x22};
  byte rhs{0x01};
  CPU::Flags f{};

  auto result = operations::decrement(lhs, rhs, f);

  EXPECT_EQ(result, byte{0x21});
  EXPECT_FALSE(f.zero);
  EXPECT_TRUE(f.negate);
  EXPECT_FALSE(f.half_carry);
  EXPECT_FALSE(f.carry);
}
} // namespace