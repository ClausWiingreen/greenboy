#include "greenboy/gameboy.hpp"

namespace greenboy {
Gameboy::Gameboy(std::unique_ptr<CPU> cpu) : m_cpu{std::move(cpu)} {}
void Gameboy::step() { m_cpu->update(); }
} // namespace greenboy
