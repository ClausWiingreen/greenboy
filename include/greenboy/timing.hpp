#pragma once
#include <chrono>

namespace greenboy {
constexpr int clockSpeed = 4194304; // cycles / second
using cycles = std::chrono::duration<int, std::ratio<1, clockSpeed>>;
} // namespace greenboy
