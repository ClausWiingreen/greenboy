#pragma once
#include <chrono>

namespace greenboy {
constexpr int ClockSpeed = 4194304; // cycles / second
using cycles = std::chrono::duration<int, std::ratio<1, ClockSpeed>>;
} // namespace greenboy
