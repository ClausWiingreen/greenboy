#pragma once

#include "timing.hpp"

namespace greenboy {
class Video {
public:
  Video() noexcept = default;
  Video(const Video &) = delete;
  Video(Video &&) = delete;

  virtual ~Video() = default;

  Video &operator=(const Video &) = delete;
  Video &operator=(Video &&) = delete;

  virtual void advance(cycles c) = 0;
};
} // namespace greenboy
