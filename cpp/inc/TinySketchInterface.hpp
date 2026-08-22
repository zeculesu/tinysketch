#pragma once

#include <cstdint>

class TinySketchInterface {
public:
  virtual ~TinySketchInterface() = default;

  virtual void update(uint32_t value) = 0;
  virtual uint32_t query(uint32_t value) = 0;
};