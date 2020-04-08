#include "greenboy/fetch_execute_cpu.hpp"

#include <stdexcept>

namespace greenboy {
FetchExecuteCPU::FetchExecuteCPU(std::unique_ptr<MemoryBus> memory,
                                 std::unique_ptr<OpcodeTranslator> controlUnit)
    : m_memory(std::move(memory)), m_controlUnit(std::move(controlUnit)) {
  if (!m_memory) {
    throw std::invalid_argument("memory may not be null");
  }
  if (!m_controlUnit) {
    throw std::invalid_argument("controlUnit may not be null");
  }
}

cycles FetchExecuteCPU::update() {
  auto opcode = m_memory->read(m_registers.pc);
  auto& instruction = m_controlUnit->translate(opcode);
  return instruction.execute(m_registers, *m_memory);
}
} // namespace greenboy
