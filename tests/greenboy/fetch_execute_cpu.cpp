#include "greenboy/fetch_execute_cpu.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {
using namespace greenboy;
using ::testing::_;
using ::testing::ReturnRef;

class MockMemoryBus : public MemoryBus {
public:
  MOCK_METHOD(byte, read, (word), (const, override));
  MOCK_METHOD(void, write, (word, byte), (override));
};

class MockOpcodeTranslator : public OpcodeTranslator {
public:
  MOCK_METHOD(Instruction &, translate, (byte), (override));
};

TEST(FetchExecuteCPUCtor, RejectsNullControlUnit) {
  ASSERT_THROW((FetchExecuteCPU{std::make_unique<MockMemoryBus>(), nullptr}),
               std::invalid_argument);
}

TEST(FetchExecuteCPUCtor, RejectsNullMemoryBus) {
  ASSERT_THROW(
      (FetchExecuteCPU{nullptr, std::make_unique<MockOpcodeTranslator>()}),
      std::invalid_argument);
}

TEST(FetchExecuteCPUUpdate, FetchesFromMemory) {
  auto memory = std::make_unique<MockMemoryBus>();
  EXPECT_CALL(*memory, read(_));
  instructions::NOP instruction;
  auto controlUnit = std::make_unique<MockOpcodeTranslator>();
  EXPECT_CALL(*controlUnit, translate(_)).WillOnce(ReturnRef(instruction));

  FetchExecuteCPU cpu{std::move(memory), std::move(controlUnit)};

  cpu.update();
}

TEST(FetchExecuteCPUUpdate, FirstFetchIsFromAddress0) {
  auto memory = std::make_unique<MockMemoryBus>();
  EXPECT_CALL(*memory, read(word{0x0000}));
  instructions::NOP instruction;
  auto controlUnit = std::make_unique<MockOpcodeTranslator>();
  EXPECT_CALL(*controlUnit, translate(_))
      .WillOnce(ReturnRef(instruction));

  FetchExecuteCPU cpu{std::move(memory), std::move(controlUnit)};

  cpu.update();
}
} // namespace
