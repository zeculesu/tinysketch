#include "TinySketchHardware.hpp"
#include "VTinySketch.h"

void TinySketchHardware::tick() {
  dut.clock = 0;
  dut.eval();

  dut.clock = 1;
  dut.eval();

  dut.clock = 0;
  dut.eval();
}

TinySketchHardware::TinySketchHardware() {
  dut.reset = 1;
  dut.io_update_valid = 0;
  dut.io_query_valid = 0;
  dut.io_value = 0;
  tick();
  dut.reset = 0;
}

void TinySketchHardware::update(uint32_t value) {
  dut.io_value = value;
  dut.io_update_valid = 1;
  tick();
  dut.io_update_valid = 0;
}

uint32_t TinySketchHardware::query(uint32_t value) {
  dut.io_value = value;
  dut.io_query_valid = 1;

  dut.eval();

  uint32_t result = dut.io_query_result;

  dut.io_query_valid = 0;
  dut.eval();

  return result;
}
