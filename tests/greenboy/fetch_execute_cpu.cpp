#include "greenboy/fetch_execute_cpu.hpp"
#include "gtest/gtest.h"

#include "mocks/instruction.hpp"
#include "mocks/memory_bus.hpp"
#include "mocks/opcode_translator.hpp"

namespace {
using namespace greenboy;
using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;

TEST(FetchExecuteCPUCtor, RejectsNullControlUnit) {
  ASSERT_THROW(FetchExecuteCPU(std::make_unique<MockMemoryBus>(), nullptr),
               std::invalid_argument);
}

TEST(FetchExecuteCPUCtor, RejectsNullMemoryBus) {
  ASSERT_THROW(
      FetchExecuteCPU(nullptr, std::make_unique<MockOpcodeTranslator>()),
      std::invalid_argument);
}

TEST(FetchExecuteCPUCtor, CallsToUpdateDelegatesToComponents) {
  auto memory = std::make_unique<MockMemoryBus>();
  auto translator = std::make_unique<MockOpcodeTranslator>();
  MockInstruction instruction;

  EXPECT_CALL(*memory, read(word{0x0000})).WillOnce(Return(byte{0x10}));
  EXPECT_CALL(*translator, translate(byte{0x10}))
      .WillOnce(ReturnRef(instruction));
  EXPECT_CALL(instruction, execute(_, _));

  FetchExecuteCPU cpu{std::move(memory), std::move(translator)};
  cpu.update();
}
} // namespace
