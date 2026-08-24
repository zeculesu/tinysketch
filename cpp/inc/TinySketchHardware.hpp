#pragma once

#include "TinySketchInterface.hpp"
#include "VTinySketch.h"

class TinySketchHardware : public TinySketchInterface {
public:
  TinySketchHardware();

  void update(uint32_t value) override;
  uint32_t query(uint32_t value) override;

  void reset();

private:
  VTinySketch dut;

  void tick();
};