#pragma once

#include "greenboy/video.hpp"
#include "gmock/gmock.h"

class MockVideo : public greenboy::Video {
public:
  MOCK_METHOD(void, advance, (greenboy::cycles c), (override));
};
