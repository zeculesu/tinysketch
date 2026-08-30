#pragma once

#include "TinySketchInterface.hpp"
#include "VTinySketch.h"

#include <memory>

class VTinySketch;

static void tick_module(VTinySketch &dut) {
  dut.clock = 0;
  dut.eval();

  dut.clock = 1;
  dut.eval();

  dut.clock = 0;
  dut.eval();
}

template <std::size_t Rows, std::size_t Columns, std::size_t CounterWidth>
class TinySketchHardware
    : public TinySketchInterface<Rows, Columns, CounterWidth> {
public:
  using value_type =
      typename TinySketchInterface<Rows, Columns, CounterWidth>::value_type;

  TinySketchHardware() : dut(std::make_unique<VTinySketch>()) {
    dut->reset = 1;
    tick_module(*dut);
    dut->reset = 0;
    dut->io_update_valid = 0;
    dut->io_query_valid = 0;
    dut->io_value = 0;
  }
  ~TinySketchHardware() override = default;

  void update(value_type value) override {
    dut->io_value = value;
    dut->io_update_valid = 1;
    tick_module(*dut);
    dut->io_update_valid = 0;
  }

  value_type query(value_type value) const override {
    dut->io_value = value;
    dut->io_query_valid = 1;

    dut->eval();

    uint32_t result = dut->io_query_result;

    dut->io_query_valid = 0;
    dut->eval();

    return result;
  }

  void reset() {
    dut->reset = 1;
    tick_module(*dut);
    dut->reset = 0;
  }

private:
  std::unique_ptr<VTinySketch> dut;
};