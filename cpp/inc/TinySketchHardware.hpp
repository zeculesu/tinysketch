#pragma once

#include "TinySketchInterface.hpp"

#include <memory>

class VTinySketch;

class TinySketchHardware : public TinySketchInterface {
public:
  TinySketchHardware();
  ~TinySketchHardware() override;

  void update(uint32_t value) override;
  uint32_t query(uint32_t value) override;

  void reset();

private:
  std::unique_ptr<VTinySketch> dut;
};