#include "greenboy/fetch_execute_cpu.hpp"

#include <cassert>

#include "greenboy/instruction.hpp"
#include "greenboy/memory_bus.hpp"
#include "greenboy/opcode_translator.hpp"

namespace greenboy {
FetchExecuteCPU::FetchExecuteCPU(
    std::unique_ptr<MemoryBus> memory,
    std::unique_ptr<OpcodeTranslator> controlUnit) noexcept
    : m_memory(std::move(memory)), m_controlUnit(std::move(controlUnit)) {
  assert(m_memory != nullptr);
  assert(m_controlUnit != nullptr);
}

cycles FetchExecuteCPU::update() {
  const auto opcode = m_memory->read(m_registers.pc);
  const auto &instruction = m_controlUnit->translate(opcode);
  return instruction.execute(m_registers, *m_memory);
}
} // namespace greenboy
